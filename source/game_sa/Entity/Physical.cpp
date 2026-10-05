/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPhysical
 * Reconstrucción: COMPLETA — 56/56 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/56 entradas.
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

#include "Physical.h"
#include "CarCtrl.h"
#include "Glass.h"
#include "TaskSimpleClimb.h"
#include "RealTimeShadowManager.h"

/**
 * Registra integración física, fuerzas, colisiones, attachments y listas espaciales.
 */
void CPhysical::InjectHooks()
{
    RH_ScopedVirtualClass(CPhysical, 0x863BA0, 23);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(Constructor, 0x542260);
    RH_ScopedInstall(Destructor, 0x542450);

    RH_ScopedInstall(RemoveAndAdd, 0x542560);
    RH_ScopedInstall(ApplyTurnForce, 0x542A50);
    RH_ScopedInstall(ApplyForce, 0x542B50);
    RH_ScopedInstall(GetSpeed, 0x542CE0);
    RH_ScopedInstall(ApplyMoveSpeed, 0x542DD0);
    RH_ScopedInstall(ApplyTurnSpeed, 0x542E20);
    RH_ScopedOverloadedInstall(ApplyMoveForce, "vec", 0x5429F0, void(CPhysical::*)(CVector force));
    RH_ScopedInstall(SetDamagedPieceRecord, 0x5428C0);
    RH_ScopedInstall(RemoveFromMovingList, 0x542860);
    RH_ScopedInstall(AddToMovingList, 0x542800);
    RH_ScopedVMTInstall(Add, 0x544A30);
    RH_ScopedVMTInstall(Remove, 0x5424C0);
    RH_ScopedVMTInstall(GetBoundRect, 0x5449B0);
    RH_ScopedVMTInstall(ProcessControl, 0x5485E0);
    RH_ScopedVMTInstall(ProcessCollision, 0x54DFB0);
    RH_ScopedVMTInstall(ProcessShift, 0x54DB10);
    RH_ScopedVMTInstall(TestCollision, 0x54DEC0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x546D00);
    RH_ScopedInstall(ApplyGravity, 0x542FE0);
    RH_ScopedInstall(ApplyFrictionMoveForce, 0x5430A0);
    RH_ScopedInstall(ApplyFrictionForce, 0x543220);
    RH_ScopedInstall(SkipPhysics, 0x5433B0);
    RH_ScopedInstall(AddCollisionRecord, 0x543490);
    RH_ScopedInstall(GetHasCollidedWith, 0x543540);
    RH_ScopedInstall(GetHasCollidedWithAnyObject, 0x543580);
    RH_ScopedOverloadedInstall(ApplyCollision, "1", 0x5435C0, bool(CPhysical::*)(CEntity*, const CColPoint&, float&));
    RH_ScopedOverloadedInstall(ApplySoftCollision, "1", 0x543890, bool(CPhysical::*)(CEntity*, const CColPoint&, float&));
    RH_ScopedInstall(ApplySpringCollision, 0x543C90);
    RH_ScopedInstall(ApplySpringCollisionAlt, 0x543D60);
    RH_ScopedInstall(ApplySpringDampening, 0x543E90);
    RH_ScopedInstall(RemoveRefsToEntity, 0x544280);
    RH_ScopedInstall(DettachEntityFromEntity, 0x5442F0);
    RH_ScopedInstall(DettachAutoAttachedEntity, 0x5446A0);
    RH_ScopedInstall(GetLightingFromCol, 0x5447B0);
    RH_ScopedInstall(GetLightingTotal, 0x544850);
    RH_ScopedInstall(CanPhysicalBeDamaged, 0x5448B0);
    RH_ScopedInstall(ApplyAirResistance, 0x544C40);
    RH_ScopedInstall(ApplyCollisionAlt, 0x544D50);
    RH_ScopedOverloadedInstall(ApplyFriction, "self", 0x5454C0, bool(CPhysical::*)(float, CColPoint&));
    RH_ScopedOverloadedInstall(ApplyFriction, "other", 0x545980, bool(CPhysical::*)(CPhysical*, float, CColPoint&));
    RH_ScopedInstall(ProcessShiftSectorList, 0x546670);
    RH_ScopedInstall(ApplySpeed, 0x547B80);
    RH_ScopedInstall(UnsetIsInSafePosition, 0x548320);
    RH_ScopedOverloadedInstall(ApplyFriction, "void", 0x5483D0, void(CPhysical::*)());
    RH_ScopedOverloadedInstall(ApplyCollision, "2", 0x548680, bool(CPhysical::*)(CEntity*, CColPoint&, float&, float&));
    RH_ScopedOverloadedInstall(ApplySoftCollision, "2", 0x54A2C0, bool(CPhysical::*)(CPhysical*, CColPoint&, float&, float&));
    RH_ScopedInstall(ProcessCollisionSectorList, 0x54BA60);
    RH_ScopedInstall(ProcessCollisionSectorList_SimpleCar, 0x54CFF0);
    RH_ScopedOverloadedInstall(AttachEntityToEntity, "vec", 0x54D570, void(CPhysical::*)(CPhysical*, CVector, CVector));
    RH_ScopedOverloadedInstall(AttachEntityToEntity, "quat", 0x54D690, void(CPhysical::*)(CPhysical*, CVector*, CQuaternion*));
    RH_ScopedInstall(CheckCollision, 0x54D920);
    RH_ScopedInstall(CheckCollision_SimpleCar, 0x54DAB0);
    RH_ScopedInstall(PlacePhysicalRelativeToOtherPhysical, 0x546DB0);
    RH_ScopedInstall(PositionAttachedEntity, 0x546FF0);
}

// 0x542260
/**
 * Materializa una matriz completa y establece un cuerpo dinámico neutro con gravedad,
 * colisión y masas por defecto.
 */
CPhysical::CPhysical() : CEntity()
{
    m_pCollisionList.m_node = nullptr;

    CPlaceable::AllocateStaticMatrix();
    m_matrix->SetUnity();

    ResetMoveSpeed();
    ResetTurnSpeed();
    m_vecFrictionMoveSpeed.Set(0.0f, 0.0f, 0.0f);
    m_vecFrictionTurnSpeed.Set(0.0f, 0.0f, 0.0f);
    m_vecForce.Set(0.0f, 0.0f, 0.0f);
    m_vecTorque.Set(0.0f, 0.0f, 0.0f);

    m_fMass = 1.0f;
    m_fTurnMass = 1.0f;
    m_fVelocityFrequency = 1.0f;
    m_fAirResistance = 0.1f;
    m_pMovingList = nullptr;
    m_nFakePhysics = 0;
    m_nNumEntitiesCollided = 0;
    std::ranges::fill(m_apCollidedEntities, nullptr);

    m_nPieceType = 0;

    m_fDamageIntensity = 0.0f;
    m_pDamageEntity = nullptr;

    m_vecLastCollisionImpactVelocity.Set(0.0f, 0.0f, 0.0f);
    m_vecLastCollisionPosn.Set(0.0f, 0.0f, 0.0f);

    SetUsesCollision(true);

    m_vecCentreOfMass.Set(0.0f, 0.0f, 0.0f);

    m_fMovingSpeed = 0.0f;
    m_pAttachedTo = nullptr;
    m_pEntityIgnoredCollision = nullptr;

    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

    m_fDynamicLighting = 0.0f;
    m_pShadowData = nullptr;
    field_38 = 100.0f;

    m_nPhysicalFlags = 0;
    physicalFlags.bApplyGravity = true;

    // NOTSA (se conserva): el EXE no escribe m_nContactSurface (0xBA); sin inicializar, el audio de peatones y vehículos
    // indexaría g_surfaceInfos fuera de rango en la DLL de depuración antes del primer contacto. El banco no compara 0xBA.
    m_nContactSurface = SURFACE_DEFAULT;
    m_fContactSurfaceBrightness = 1.0f;
}

// 0x542450
CPhysical::~CPhysical()
{
    if (m_pShadowData)
        g_realTimeShadowMan.ReturnRealTimeShadow(m_pShadowData);

    m_pCollisionList.Flush();
}

namespace {
// The executable multiplies by the stored float reciprocal in x87 before
// floor(double). Rounding to float, or dividing by 50, changes sector borders.
int32 PhysicalSector(float coordinate) {
    const float reciprocal = .02f, origin = 60.f;
    double sector;
    __asm {
        fld coordinate
        fmul reciprocal
        fadd origin
        fstp sector
    }
    return static_cast<int32>(std::floor(sector));
}
CPtrListDoubleLink<CPhysical*>* PhysicalSectorList(CRepeatSector& sector, eEntityType type) {
    switch (type) {
    case ENTITY_TYPE_VEHICLE: return reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(&sector.Vehicles);
    case ENTITY_TYPE_PED:     return reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(&sector.Peds);
    case ENTITY_TYPE_OBJECT:  return reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(&sector.Objects);
    default: NOTSA_UNREACHABLE();
    }
}
}

// 0x544A30
/**
 * Inserta el cuerpo en cada repeat-sector tocado por su bound y crea un CEntryInfoNode
 * por inserción para poder retirarlo sin búsquedas posteriores.
 */
void CPhysical::Add() {
    if (m_bIsBIGBuilding) {
        CEntity::Add();
        return;
    }
    const auto bounds = GetBoundRect();
    const auto minX = PhysicalSector(bounds.left), minY = PhysicalSector(bounds.bottom);
    const auto maxX = PhysicalSector(bounds.right), maxY = PhysicalSector(bounds.top);
    for (auto y = minY; y <= maxY; ++y) {
        for (auto x = minX; x <= maxX; ++x) {
            auto* sector = &CWorld::ms_aRepeatSectors[uint32(y) & 15u][uint32(x) & 15u];
            auto* list = PhysicalSectorList(*sector, GetType());
            auto* entry = new CEntryInfoNode;
            entry->m_doubleLink = list->AddItem(this);
            entry->m_repeatSector = sector;
            entry->m_doubleLinkList = list;
            entry->AddToList(m_pCollisionList.m_node);
            m_pCollisionList.m_node = entry;
        }
    }
}

// 0x5424C0
// Usa m_pCollisionList para eliminar todos los nodos espaciales creados por Add().
void CPhysical::Remove()
{
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
        return;
    }

    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    while (entryInfoNode) {
        CEntryInfoNode* nextEntryInfoNode = entryInfoNode->m_next;
        entryInfoNode->m_doubleLinkList->DeleteNode(entryInfoNode->m_doubleLink);
        m_pCollisionList.DeleteNode(entryInfoNode);

        entryInfoNode = nextEntryInfoNode;
    }
}

// 0x5449B0
CRect CPhysical::GetBoundRect() const {
    CVector boundCentre = CEntity::GetBoundCentre();
    const float fRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
    return CRect(
        boundCentre.x - fRadius, 
        boundCentre.y - fRadius, 
        boundCentre.x + fRadius, 
        boundCentre.y + fRadius
    );
}

// 0x5485E0
/**
 * Reinicia estado transitorio del frame, limpia daños/contactos y acumula gravedad,
 * resistencia y fricción antes de la fase de aplicación de velocidades.
 */
void CPhysical::ProcessControl()
{
    if (!GetIsTypePed())
        physicalFlags.bSubmergedInWater = false;

    SetHasHitWall(false);
    SetWasPostponed(false);
    SetIsInSafePosition(false);
    SetHasContacted(false);

    if (GetStatus() != STATUS_SIMPLE)
    {
        physicalFlags.bDoorHitEndStop = false;
        physicalFlags.bOnSolidSurface = false;
        m_nNumEntitiesCollided = 0;
        m_nPieceType = 0;
        m_fDamageIntensity = 0.0f;
        CEntity::SafeCleanUpRef(m_pDamageEntity);
        m_pDamageEntity = nullptr;
        ApplyFriction();
        if (!m_pAttachedTo || physicalFlags.bInfiniteMass)
        {
            ApplyGravity();
            ApplyAirResistance();
        }
    }
}

namespace {
CVector PhysAttachMatPoint59C890(const CMatrix& m, const CVector& v); // Definida junto a PlacePhysicalRelativeToOtherPhysical
}

// 0x54DFB0
/**
 * Orquesta la detección/resolución de colisiones según status y subtipo.
 *
 * La función maneja rutas especiales de simple car/ghost/vehículo y puede cambiar
 * STATUS_SIMPLE a física completa cuando aparece una colisión relevante.
 */
void CPhysical::ProcessCollision() {
    ZoneScoped;

    auto* vehicle    = AsVehicle();
    auto* automobile = AsAutomobile();
    auto* bike       = AsBike();

    m_fMovingSpeed = 0.0f;
    physicalFlags.bProcessingShift = false;
    physicalFlags.bSkipLineCol = false;
    if (GetUsesCollision() && !physicalFlags.bDisableSimpleCollision) {
        if (GetStatus() == STATUS_SIMPLE) {
            if (CheckCollision_SimpleCar() && GetStatus() == STATUS_SIMPLE) {
                SetStatus(STATUS_PHYSICS);
                if (GetIsTypeVehicle())
                    CCarCtrl::SwitchVehicleToRealPhysics(vehicle);
            }
            SetIsStuck(false);
            SetIsInSafePosition(true);
            RemoveAndAdd();
            return;
        }

        // TODO:
        // Refactor this to be a lambda that takes in these variables as parameters,
        // this way we keep the array reference (so oob checks in debug still work and shit)
        if (GetStatus() == STATUS_GHOST) {
            CColPoint* wheelsColPoints = nullptr;
            float* pfWheelsSuspensionCompression = nullptr;
            CVector* wheelsCollisionPositions = nullptr;
            if (vehicle->m_nVehicleSubType) { // todo: m_nVehicleSubType
                bike->m_aGroundPhysicalPtrs[0] = nullptr; // todo: enum
                bike->m_aGroundPhysicalPtrs[1] = nullptr;
                bike->m_aGroundPhysicalPtrs[2] = nullptr;
                bike->m_aGroundPhysicalPtrs[3] = nullptr;
                wheelsColPoints = bike->m_aWheelColPoints.data();
                pfWheelsSuspensionCompression = bike->m_aWheelRatios.data();
                wheelsCollisionPositions = bike->m_aGroundOffsets.data();
            }
            else {
                automobile->m_apWheelCollisionEntity[0] = nullptr;
                automobile->m_apWheelCollisionEntity[1] = nullptr;
                automobile->m_apWheelCollisionEntity[2] = nullptr;
                automobile->m_apWheelCollisionEntity[3] = nullptr;

                // TODO: Use std::array<> for these local variables
                wheelsColPoints = automobile->m_wheelColPoint.data();
                pfWheelsSuspensionCompression = automobile->m_fWheelsSuspensionCompression.data();
                wheelsCollisionPositions = automobile->m_vWheelCollisionPos.data();
            }
            CCollisionData* colData = GetColModel()->m_pColData;
            int32 collisionIndex = 0;
            while (true) {
                int32 colLinesCount = static_cast<int8>(colData->m_nNumLines); // Como el original: movsx (byte con signo)
                if (colData->bUsesDisks)
                    colLinesCount = 0;
                if (collisionIndex >= colLinesCount) {
                    SetIsStuck(false);
                    SetIsInSafePosition(true);
                    m_vecMoveSpeed.z = 0.0f;
                    m_vecTurnSpeed.x = 0.0f;
                    m_vecTurnSpeed.y = 0.0f;
                    m_matrix->ForceUpVector(CVector(0.0f, 0.0f, 1.0f));
                    // Como el original: primero la altura sobre la carretera (virtual) y luego FindGhostRoadHeight
                    const float fHeightAboveRoad = vehicle->GetHeightAboveRoad();
                    GetPosition().z = CCarCtrl::FindGhostRoadHeight(vehicle) + fHeightAboveRoad;
                    ApplySpeed();
                    m_matrix->Reorthogonalise();
                    RemoveAndAdd();
                    vehicle->vehicleFlags.bVehicleColProcessed = true;
                    return;
                }

                CColPoint* wheelColPoint = &wheelsColPoints[collisionIndex];
                CColLine* colLine = &colData->m_pLines[collisionIndex];
                // Como el original: 0x59C890 con su orden de sumas
                CVector vecColLinePosStart = PhysAttachMatPoint59C890(*m_matrix, colLine->m_vecStart);
                CVector vecColLinePosEnd = PhysAttachMatPoint59C890(*m_matrix, colLine->m_vecEnd);
                wheelColPoint->m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
                wheelColPoint->m_nSurfaceTypeA = SURFACE_WHEELBASE;
                wheelColPoint->m_nSurfaceTypeB = SURFACE_TARMAC;
                wheelColPoint->m_fDepth = 0.0f;
                float fGhostRoadHeight = CCarCtrl::FindGhostRoadHeight(vehicle);
                if (!(fGhostRoadHeight > vecColLinePosStart.z)) { // Como el original: NaN no entra en la rama de 0
                    if (fGhostRoadHeight > vecColLinePosEnd.z) {
                        float fWheelSuspensionCompression = (vecColLinePosStart.z - fGhostRoadHeight) / (vecColLinePosStart.z - vecColLinePosEnd.z);
                        pfWheelsSuspensionCompression[collisionIndex] = fWheelSuspensionCompression;
                        wheelColPoint->m_vecPoint = (vecColLinePosEnd - vecColLinePosStart) * fWheelSuspensionCompression + vecColLinePosStart;
                    }
                    else {
                        pfWheelsSuspensionCompression[collisionIndex] = 1.0f;
                        wheelColPoint->m_vecPoint = vecColLinePosEnd;
                    }
                }
                else {
                    pfWheelsSuspensionCompression[collisionIndex] = 0.0f;
                    wheelColPoint->m_vecPoint = vecColLinePosStart;
                }
                wheelsCollisionPositions[collisionIndex] = wheelColPoint->m_vecPoint - GetPosition();
                collisionIndex++;
            }
        }
        CVector vecOldMoveSpeed = m_vecMoveSpeed;
        float fOldTimeStep = CTimer::GetTimeStep();
        float fOldElasticity = m_fElasticity;
        CMatrix oldEntityMatrix(*m_matrix);
        bool bProcessCollisionBeforeSettingTimeStep = false;
        bool bUnknown = false;
        uint8 collisionSteps = SpecialEntityCalcCollisionSteps(bProcessCollisionBeforeSettingTimeStep, bUnknown);
        float fStep = fOldTimeStep / collisionSteps;
        if (bProcessCollisionBeforeSettingTimeStep)
        {
            ApplySpeed();
            m_matrix->Reorthogonalise();
            physicalFlags.bProcessingShift = false;
            physicalFlags.bSkipLineCol = false;
            physicalFlags.bForceHitReturnFalse = true;
            bool bOldUsesCollision = GetUsesCollision();
            SetUsesCollision(false);
            if (!CheckCollision())
            {
                physicalFlags.bForceHitReturnFalse = false;
                SetUsesCollision(bOldUsesCollision);
                if (GetIsTypeVehicle())
                    vehicle->vehicleFlags.bVehicleColProcessed = true;
                SetIsStuck(false);
                SetIsInSafePosition(true);
                physicalFlags.bProcessCollisionEvenIfStationary = false;
                physicalFlags.bSkipLineCol = false;
                m_fElasticity = fOldElasticity;
                m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
                RemoveAndAdd();
                return;
            }
            SetUsesCollision(bOldUsesCollision);
            physicalFlags.bForceHitReturnFalse = false;
            *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
            m_vecMoveSpeed = vecOldMoveSpeed;
            if (GetIsTypeVehicle() && vehicle->vehicleFlags.bIsLawEnforcer)
                m_fElasticity *= HIGHSPEED_ELASTICITY_MULT_COPCAR;
        }

        CPed* ped = this->AsPed();
        if (collisionSteps > 1u)
        {
            for (uint8 stepIndex = 1; stepIndex < collisionSteps; stepIndex++)
            {
                CTimer::UpdateTimeStep(stepIndex * fStep);
                ApplySpeed();
                bool bCheckCollision = CheckCollision();
                if (GetIsTypePed() && m_vecMoveSpeed.z == 0.0f && !ped->bWasStanding && ped->bIsStanding) {
                    oldEntityMatrix.GetPosition().z = GetPosition().z;
                }
                *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
                CTimer::UpdateTimeStep(fOldTimeStep);
                if (bCheckCollision) {
                    m_fElasticity = fOldElasticity;
                    return;
                }
                if (GetIsTypeVehicle()) {
                    if (vehicle->m_nVehicleType) { // todo: m_nVehicleType
                        if (vehicle->IsBike()) {
                            bike->m_aWheelRatios[0] = 1.0f; // todo: enum
                            bike->m_aWheelRatios[1] = 1.0f;
                            bike->m_aWheelRatios[2] = 1.0f;
                            bike->m_aWheelRatios[3] = 1.0f;
                        }
                        else if (vehicle->IsTrailer()) {
                            automobile->m_fWheelsSuspensionCompression[0] = 1.0f;
                            automobile->m_fWheelsSuspensionCompression[1] = 1.0f;
                            automobile->m_fWheelsSuspensionCompression[2] = 1.0f;
                        }
                    }
                    else {
                        automobile->m_fWheelsSuspensionCompression[0] = 1.0f;
                        automobile->m_fWheelsSuspensionCompression[1] = 1.0f;
                        automobile->m_fWheelsSuspensionCompression[2] = 1.0f;
                        automobile->m_fWheelsSuspensionCompression[3] = 1.0f;
                    }
                }
            }
        }

        ApplySpeed();
        m_matrix->Reorthogonalise();
        physicalFlags.bProcessingShift = false;
        physicalFlags.bSkipLineCol = false;
        if (   m_vecMoveSpeed != 0.0f
            || m_vecTurnSpeed != 0.0f
            || physicalFlags.bProcessCollisionEvenIfStationary
            || GetStatus() == STATUS_PLAYER
            || GetIsTypeVehicle() && vehicle->vehicleFlags.bRestingOnPhysical // Como el original: bit 1 de +0x42C (bRestingOnPhysical), no bIsCarParkVehicle
            || GetIsTypePed() && (ped->IsPlayer() || ped->bTestForBlockedPositions|| !ped->bIsStanding))
        {
            if (GetIsTypeVehicle())
                vehicle->vehicleFlags.bVehicleColProcessed = true;
            if (CheckCollision()) {
                *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
                m_fElasticity = fOldElasticity;
                return;
            }
        }
        else if (GetIsTypePed()) {
            ped->bIsStanding = true;
        }
        SetIsStuck(false);
        SetIsInSafePosition(true);
        physicalFlags.bProcessCollisionEvenIfStationary = false;
        physicalFlags.bSkipLineCol = false;
        m_fElasticity = fOldElasticity;
        m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
        RemoveAndAdd();
        return;
    }
    SetIsStuck(false);
    SetIsInSafePosition(true);
    RemoveAndAdd();
}

// 0x54DB10
void CPhysical::ProcessShift() {
    ZoneScoped;

    CRect boundingBox = GetBoundRect();
    m_fMovingSpeed = 0.0f;

    bool bPhysicalFlagsSet = m_nPhysicalFlags & (PHYSICAL_DISABLE_MOVE_FORCE | PHYSICAL_INFINITE_MASS | PHYSICAL_DISABLE_Z);
    if (GetStatus() == STATUS_SIMPLE || bPhysicalFlagsSet)
    {
        if (bPhysicalFlagsSet)
        {
            ResetTurnSpeed();
        }
        SetIsStuck(false);
        SetIsInSafePosition(true);
        RemoveAndAdd();
    }
    else
    {
        if (GetHasHitWall())
        {
            // Como el original: el exe lee los physicalFlags (+0x40) de la entidad bajo el peatón:
            // amortigua si no hay entidad, si no tiene bDisableCollisionForce o si tiene bCollidable.
            CPed* ped = AsPed();
            bool bSomeSpecificFlagsSet = false;
            if (GetIsTypePed()) {
                CEntity* standingOn = ped->m_standingOnEntity;
                if (!standingOn) {
                    bSomeSpecificFlagsSet = true;
                } else {
                    const uint32 standingOnFlags = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(standingOn) + 0x40);
                    if (!(standingOnFlags & PHYSICAL_DISABLE_COLLISION_FORCE) || (standingOnFlags & PHYSICAL_COLLIDABLE))
                        bSomeSpecificFlagsSet = true;
                }
            }
            if (bSomeSpecificFlagsSet || CWorld::bSecondShift)
            {
                // Como el original: pow(damp, paso) con el _CIpow del EXE (0x822130), redondeado a float
                const float fDamp = CPhysical::PHYSICAL_SHIFT_SPEED_DAMP;
                const float fStep = CTimer::GetTimeStep();
                float fMoveSpeedShift;
                __asm {
                    fld fDamp
                    fld fStep
                    mov eax, 0x822130
                    call eax
                    fstp fMoveSpeedShift
                }
                m_vecMoveSpeed *= fMoveSpeedShift;
                const float fDamp2 = CPhysical::PHYSICAL_SHIFT_SPEED_DAMP;
                const float fStep2 = CTimer::GetTimeStep();
                float fTurnSpeedShift;
                __asm {
                    fld fDamp2
                    fld fStep2
                    mov eax, 0x822130
                    call eax
                    fstp fTurnSpeedShift
                }
                m_vecTurnSpeed *= fTurnSpeedShift;
            }
        }

        CMatrix oldEntityMatrix(*m_matrix);
        ApplySpeed();
        m_matrix->Reorthogonalise();
        CWorld::AdvanceCurrentScanCode();

        bool bShifted = false;
        if (GetIsTypeVehicle())
        {
            physicalFlags.bProcessingShift = true;
        }

        // todo: shadow var
        // Como el original: coordenada * 0.02 + 60 en x87 y floor(double) (PhysicalSector), no / 50
        int32 startSectorX = PhysicalSector(boundingBox.left);
        int32 startSectorY = PhysicalSector(boundingBox.bottom);
        int32 endSectorX = PhysicalSector(boundingBox.right);
        int32 endSectorY = PhysicalSector(boundingBox.top);
        for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
            for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
                if (ProcessShiftSectorList(sectorX, sectorY))
                    bShifted = true;
            }
        }
        physicalFlags.bProcessingShift = false;

        if (bShifted || GetIsTypeVehicle()) {
            CWorld::AdvanceCurrentScanCode();
            bool bShifted2 = false;
            int32 startSectorX = PhysicalSector(boundingBox.left);
            int32 startSectorY = PhysicalSector(boundingBox.bottom);
            int32 endSectorX = PhysicalSector(boundingBox.right);
            int32 endSectorY = PhysicalSector(boundingBox.top);
            for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
                for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
                    if (ProcessCollisionSectorList(sectorX, sectorY)) {
                        if (!CWorld::bSecondShift) {
                            *(CMatrix*)m_matrix = oldEntityMatrix;
                            return;
                        }
                        bShifted2 = true;
                    }
                }
            }
            if (bShifted2)
            {
                *(CMatrix*)m_matrix = oldEntityMatrix;
                return;
            }
        }
        SetIsStuck(false);
        SetIsInSafePosition(true);
        m_fMovingSpeed = (GetPosition() - oldEntityMatrix.GetPosition()).Magnitude();
        RemoveAndAdd();
    }
}

// 0x54DEC0
bool CPhysical::TestCollision(bool bApplySpeed) {
    CMatrix entityMatrix(*m_matrix);
    physicalFlags.bForceHitReturnFalse = true;
    physicalFlags.bSkipLineCol = true;
    bool bOldUsesCollision = GetUsesCollision();
    SetUsesCollision(false);
    bool bTestForBlockedPositions = false;

    CPed* ped = AsPed();
    if (GetIsTypePed() && ped->bTestForBlockedPositions) {
        bTestForBlockedPositions = true;
        ped->bTestForBlockedPositions = false;
    }

    if (bApplySpeed)
        ApplySpeed();

    bool bCheckCollision = CheckCollision();
    SetUsesCollision(bOldUsesCollision);
    physicalFlags.bForceHitReturnFalse = false;
    physicalFlags.bSkipLineCol = false;
    *(CMatrix*)m_matrix = entityMatrix;
    if (bTestForBlockedPositions)
        ped->bTestForBlockedPositions = true;

    return bCheckCollision;
}

// 0x546D00
int32 CPhysical::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    CColModel* colModel = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
    int32 totalColPointsToProcess = CCollision::ProcessColModels(*m_matrix, *colModel, entity->GetMatrix(), *entity->GetColModel(), *(std::array<CColPoint, 32>*)colPoint/*should be okay for now*/, nullptr, nullptr, false);
    if (totalColPointsToProcess > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding())
            entity->AsPhysical()->AddCollisionRecord(this);

        if (entity->GetIsTypeBuilding() || entity->GetIsStatic())
            SetHasHitWall(true);
    }
    return totalColPointsToProcess;
}

// 0x542560
void CPhysical::RemoveAndAdd() {
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
        CEntity::Add();
        return;
    }
    auto* entry = m_pCollisionList.m_node;
    const auto bounds = GetBoundRect();
    const auto minX = PhysicalSector(bounds.left), minY = PhysicalSector(bounds.bottom);
    const auto maxX = PhysicalSector(bounds.right), maxY = PhysicalSector(bounds.top);
    for (auto y = minY; y <= maxY; ++y) {
        for (auto x = minX; x <= maxX; ++x) {
            auto* sector = &CWorld::ms_aRepeatSectors[uint32(y) & 15u][uint32(x) & 15u];
            auto* list = PhysicalSectorList(*sector, GetType());
            if (entry) {
                entry->m_doubleLinkList->UnlinkNode(entry->m_doubleLink);
                list->AddNode(entry->m_doubleLink);
                entry->m_repeatSector = sector;
                entry->m_doubleLinkList = list;
                entry = entry->m_next;
            } else {
                // The original allocates the pointer node before the entry node here.
                auto* link = list->AddItem(this);
                auto* added = new CEntryInfoNode;
                added->m_doubleLink = link;
                added->m_repeatSector = sector;
                added->m_doubleLinkList = list;
                added->AddToList(m_pCollisionList.m_node);
                m_pCollisionList.m_node = added;
            }
        }
    }
    while (entry) {
        auto* next = entry->m_next;
        entry->m_doubleLinkList->DeleteNode(entry->m_doubleLink);
        m_pCollisionList.DeleteNode(entry);
        entry = next;
    }
}

// 0x542800
void CPhysical::AddToMovingList()
{
    if (!m_pMovingList && !m_bIsStaticWaitingForCollision)
    {
        auto pLink = CWorld::ms_listMovingEntityPtrs.AddItem(this);
        m_pMovingList = pLink;
    }
}

// 0x542860
void CPhysical::RemoveFromMovingList()
{
    if (m_pMovingList) {
        CWorld::ms_listMovingEntityPtrs.DeleteNode(m_pMovingList);
        m_pMovingList = nullptr;
    }
}

// 0x5428C0
void CPhysical::SetDamagedPieceRecord(float fDamageIntensity, CEntity* entity, const CColPoint& colPoint, float fDistanceMult)
{
    auto* object = AsObject();
    if (fDamageIntensity > m_fDamageIntensity) {
        m_fDamageIntensity = fDamageIntensity;
        m_nPieceType = colPoint.m_nPieceTypeA;

        // CEntity::ChangeEntityReference?
        if (m_pDamageEntity)
            m_pDamageEntity->CleanUpOldReference(&m_pDamageEntity);
        m_pDamageEntity = entity;
        entity->RegisterReference(&m_pDamageEntity);

        m_vecLastCollisionPosn = colPoint.m_vecPoint;
        m_vecLastCollisionImpactVelocity = fDistanceMult * colPoint.m_vecNormal;
        if (GetIsTypeObject() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
            object->objectFlags.bDamaged = 1;
        else if (entity->GetIsTypeObject() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
            entity->AsObject()->objectFlags.bDamaged = 1;
    }

    if (physicalFlags.bDisableZ) {
        if (entity->m_nModelIndex == ModelIndices::MI_POOL_CUE_BALL && GetIsTypeObject()) {
            object->m_nLastWeaponDamage = object->m_nLastWeaponDamage != 255 ? WEAPON_RUNOVERBYCAR : WEAPON_DROWNING;
        }
    }
}

// 0x4ABBA0
void CPhysical::ApplyMoveForce(float x, float y, float z)
{
    return ApplyMoveForce(CVector(x, y ,z));
}

// 0x5429F0
void CPhysical::ApplyMoveForce(CVector force)
{
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bDisableZ)
            force.z = 0.0f;
        // Como el original: multiplica por el recíproco 1/masa en vez de dividir
        const float invMass = 1.0f / m_fMass;
        m_vecMoveSpeed += force * invMass;
    }
}

// 0x542A50
void CPhysical::ApplyTurnForce(CVector force, CVector point) {
    if (physicalFlags.bDisableTurnForce) {
        return;
    }

    if (physicalFlags.bDisableMoveForce) {
        point.z = 0.0f;
        force.z = 0.0f;
    }

    // Adjust point to be relative to the centre-of-mass
    if (!physicalFlags.bInfiniteMass)  {
        // Como el original: Multiply3x3 (0x59C790) con su orden de sumas
        CVector com;
        com.FromMultiply3x3(*m_matrix, m_vecCentreOfMass);
        point -= com;
    }

    // Apply angular velocity around this point now
    // Como el original: multiplica por el recíproco 1/turnMass
    const float invTurnMass = 1.0f / m_fTurnMass;
    m_vecTurnSpeed += CrossProduct(point, force) * invTurnMass;
}

// 0x542B50
void CPhysical::ApplyForce(CVector vecForce, CVector point, bool bUpdateTurnSpeed)
{
    CVector vecMoveSpeedForce = vecForce;
    if (physicalFlags.bDisableZ)
        vecMoveSpeedForce.z = 0.0f;

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        // Como el original: multiplica por el recíproco 1/masa
        const float invMass = 1.0f / m_fMass;
        m_vecMoveSpeed += vecMoveSpeedForce * invMass;
    }

    if (!physicalFlags.bDisableTurnForce && bUpdateTurnSpeed) {
        CVector vecCentreOfMassMultiplied{};
        float fTurnMass = m_fTurnMass;
        if (physicalFlags.bInfiniteMass)
            fTurnMass += m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f;
        else
            vecCentreOfMassMultiplied.FromMultiply3x3(*m_matrix, m_vecCentreOfMass); // Como el original: Multiply3x3 (0x59C790)

        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            vecForce.z = 0.0f;
        }

        CVector distance = point - vecCentreOfMassMultiplied;
        // Como el original: multiplica por el recíproco 1/turnMass
        const float invTurnMass = 1.0f / fTurnMass;
        m_vecTurnSpeed += CrossProduct(distance, vecForce) * invTurnMass;
    }
}

// 0x542CE0
CVector CPhysical::GetSpeed(CVector point)
{
    CVector vecCentreOfMassMultiplied{};
    if (!physicalFlags.bInfiniteMass)
        vecCentreOfMassMultiplied.FromMultiply3x3(*m_matrix, m_vecCentreOfMass); // Como el original: Multiply3x3 (0x59C790)

    CVector distance = point - vecCentreOfMassMultiplied;
    CVector vecTurnSpeed = m_vecTurnSpeed + m_vecFrictionTurnSpeed;
    CVector cross = CrossProduct(vecTurnSpeed, distance);
    // Como el original: (moveSpeed + cruz) + frictionMoveSpeed
    return CVector{
        (m_vecMoveSpeed.x + cross.x) + m_vecFrictionMoveSpeed.x,
        (m_vecMoveSpeed.y + cross.y) + m_vecFrictionMoveSpeed.y,
        (m_vecMoveSpeed.z + cross.z) + m_vecFrictionMoveSpeed.z
    };
}

void CPhysical::ApplyMoveSpeed()
{
    if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce)
        ResetMoveSpeed();
    else
        GetPosition() += CTimer::GetTimeStep() * m_vecMoveSpeed;
}

// 0x542E20
void CPhysical::ApplyTurnSpeed()
{
    if (physicalFlags.bDontApplySpeed) {
        ResetTurnSpeed();
    }
    else
    {
        CVector vecTurnSpeedTimeStep = CTimer::GetTimeStep() * m_vecTurnSpeed;
        CVector vecCrossProduct;
        CrossProduct(&vecCrossProduct, &vecTurnSpeedTimeStep, &GetRight());
        GetRight() += vecCrossProduct;
        CrossProduct(&vecCrossProduct, &vecTurnSpeedTimeStep, &GetForward());
        GetForward() += vecCrossProduct;
        CrossProduct(&vecCrossProduct, &vecTurnSpeedTimeStep, &GetUp());
        GetUp() += vecCrossProduct;
        if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
            // Como el original: fchs del centro de masas y Multiply3x3 (0x59C790)
            CVector vecNegativeCentreOfMass = -m_vecCentreOfMass;
            CVector vecCentreOfMassMultiplied;
            vecCentreOfMassMultiplied.FromMultiply3x3(*m_matrix, vecNegativeCentreOfMass);
            GetPosition() += CrossProduct(vecTurnSpeedTimeStep, vecCentreOfMassMultiplied);
        }
    }
}

// 0x542FE0
void CPhysical::ApplyGravity()
{
    if (physicalFlags.bApplyGravity && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bInfiniteMass) {
            float fMassTimeStep = CTimer::GetTimeStep() * m_fMass;
            CVector point;
            point.FromMultiply3x3(*m_matrix, m_vecCentreOfMass); // Como el original: Multiply3x3 (0x59C790)
            CVector force (0.0f, 0.0f, fMassTimeStep * -0.008f);
            ApplyForce(force, point, true);
        }
        else if (GetUsesCollision()) {
            m_vecMoveSpeed.z -= CTimer::GetTimeStep() * 0.008f;
        }
    }
}

// 0x5430A0
void CPhysical::ApplyFrictionMoveForce(CVector moveForce)
{
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce)
    {
        if (physicalFlags.bDisableZ)
        {
            moveForce.z = 0.0f;
        }
        // Como el original: multiplica por el recíproco 1/masa
        const float invMass = 1.0f / m_fMass;
        m_vecFrictionMoveSpeed += moveForce * invMass;
    }
}

// Unused
// 0x543100
void CPhysical::ApplyFrictionTurnForce(CVector posn, CVector velocity)
{
    ((void(__thiscall*)(CPhysical*, CVector, CVector))0x543100)(this, posn, velocity);
}

// 0x543220
void CPhysical::ApplyFrictionForce(CVector vecMoveForce, CVector point)
{
    CVector vecTheMoveForce = vecMoveForce;

    if (physicalFlags.bDisableZ)
    {
        vecTheMoveForce.z = 0.0f;
    }

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce)
    {
        // Como el original: multiplica por el recíproco 1/masa
        const float invMass = 1.0f / m_fMass;
        m_vecFrictionMoveSpeed += vecTheMoveForce * invMass;
    }

    CVector vecCentreOfMassMultiplied{};
    if (!physicalFlags.bDisableTurnForce)
    {
        float fTurnMass = m_fTurnMass;
        if (physicalFlags.bInfiniteMass)
            fTurnMass += m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f;
        else
            vecCentreOfMassMultiplied.FromMultiply3x3(*m_matrix, m_vecCentreOfMass); // Como el original: Multiply3x3 (0x59C790)

        if (physicalFlags.bDisableMoveForce)
        {
            point.z = 0.0f;
            vecMoveForce.z = 0.0f;
        }

        CVector vecDifference = point - vecCentreOfMassMultiplied;
        CVector vecMoveForceCrossProduct = CrossProduct(vecDifference, vecMoveForce);

        // Como el original: multiplica por el recíproco 1/turnMass
        const float invTurnMass = 1.0f / fTurnMass;
        m_vecFrictionTurnSpeed += vecMoveForceCrossProduct * invTurnMass;
    }
}

// 0x5433B0
void CPhysical::SkipPhysics()
{
    if (!GetIsTypePed() && !GetIsTypeVehicle())
        physicalFlags.bSubmergedInWater = false;

    SetHasHitWall(false);
    SetWasPostponed(false);
    SetIsInSafePosition(false);
    SetHasContacted(false);

    if (GetStatus() != STATUS_SIMPLE)
    {
        physicalFlags.bOnSolidSurface = false;
        m_nNumEntitiesCollided = 0;
        m_nPieceType = 0;
        m_fDamageIntensity = 0.0f;
        CEntity::ClearReference(m_pDamageEntity);
        ResetFrictionTurnSpeed();
        ResetFrictionMoveSpeed();
    }
}

// 0x543490
void CPhysical::AddCollisionRecord(CEntity* collidedEntity)
{
    physicalFlags.bOnSolidSurface = true;
    m_nLastCollisionTime = CTimer::GetTimeInMS();
    if (GetIsTypeVehicle())
    {
        auto* vehicle = AsVehicle();
        if (collidedEntity->GetIsTypeVehicle())
        {
            auto* collidedVehicle = collidedEntity->AsVehicle();
            // Como el original: compara el uint16 con 0xFFFF (con -1 nunca coincidía tras la promoción a int)
            if (vehicle->m_nAlarmState == 0xFFFF)
                vehicle->m_nAlarmState = 15000;
            if (collidedVehicle->m_nAlarmState == 0xFFFF)
                collidedVehicle->m_nAlarmState = 15000;
        }
    }

    if (physicalFlags.bCanBeCollidedWith)
    {
        for (uint32 i = 0; i < m_nNumEntitiesCollided; i++)
        {
            if (m_apCollidedEntities[i] == collidedEntity)
                return;
        }

        if (m_nNumEntitiesCollided < std::size(m_apCollidedEntities)) {
            m_apCollidedEntities[m_nNumEntitiesCollided] = collidedEntity;
            m_nNumEntitiesCollided++;
        }
    }
}

// 0x543540
bool CPhysical::GetHasCollidedWith(CEntity* entity)
{
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0)
        return false;

    for (uint32 i = 0; i < m_nNumEntitiesCollided; i++)
    {
        if (m_apCollidedEntities[i] == entity)
            return true;
    }
    return false;
}

// 0x543580
bool CPhysical::GetHasCollidedWithAnyObject()
{
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0)
        return false;

    for (uint32 i = 0; i < m_nNumEntitiesCollided; i++)
    {
        CEntity* pCollidedEntity = m_apCollidedEntities[i];
        if (pCollidedEntity && pCollidedEntity->GetIsTypeObject())
            return true;
    }
    return false;
}

namespace {
// Como el original: Multiply3x3 (0x59C790) suma x = (up.x*z + fwd.x*y) + right.x*x,
// y = (up.y*z + right.y*x) + fwd.y*y, z = (up.z*z + right.z*x) + fwd.z*y.
CVector ApplyColMultiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y
    };
}
// Como el original: (x*x + y*y) + z*z (0x406DA0 inlined)
float ApplyColSqMag(const CVector& v) {
    return (v.x * v.x + v.y * v.y) + v.z * v.z;
}
// Como el original: CVector / float (0x4119D0) multiplica por el recíproco redondeado a float
CVector ApplyColDivide(const CVector& v, float divisor) {
    const float recip = 1.0f / divisor;
    return CVector{ recip * v.x, recip * v.y, recip * v.z };
}
// Como el original: sqrt((x*x + z*z) + y*y) (ApplyFriction 0x545980)
float ApplyColMagXZY(const CVector& v) {
    return std::sqrt((v.x * v.x + v.z * v.z) + v.y * v.y);
}
// Como el original: sqrt((z*z + y*y) + x*x)
float ApplyColMagZYX(const CVector& v) {
    return std::sqrt((v.z * v.z + v.y * v.y) + v.x * v.x);
}
} // namespace

// 0x5435C0
bool CPhysical::ApplyCollision(CEntity* entity, const CColPoint& colPoint, float& damageIntensity) {
    if (physicalFlags.bDisableTurnForce) {
        float fSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        if (fSpeedDotProduct < 0.0f)
        {
            damageIntensity = -(fSpeedDotProduct * m_fMass);
            CVector vecMoveSpeed = damageIntensity * colPoint.m_vecNormal;
            ApplyMoveForce(vecMoveSpeed.x, vecMoveSpeed.y, vecMoveSpeed.z);

            float fCollisionImpact1 = damageIntensity / m_fMass;
            AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            return true;
        }
    } else {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - GetPosition();
        CVector vecSpeed = GetSpeed(vecDistanceToPoint);

        CVector vecMoveDirection = colPoint.m_vecNormal;
        // Como el original: (n.y*s.y + n.x*s.x) + s.z*n.z
        float fSpeedDotProduct = (vecMoveDirection.y * vecSpeed.y + vecMoveDirection.x * vecSpeed.x) + vecSpeed.z * vecMoveDirection.z;
        if (fSpeedDotProduct < 0.0f)
        {
            CVector vecCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);
            CVector vecDifference = vecDistanceToPoint - vecCentreOfMassMultiplied;
            CVector vecSpeedCrossProduct = CrossProduct(vecDifference, vecMoveDirection);
            // Como el original: magnitud al cuadrado del producto cruz (el DLL usaba la de la normal)
            float fSquaredMagnitude = ApplyColSqMag(vecSpeedCrossProduct);
            float fCollisionMass = 1.0f / (fSquaredMagnitude / m_fTurnMass + 1.0f / m_fMass);

            damageIntensity = -((m_fElasticity + 1.0f) * fCollisionMass * fSpeedDotProduct);

            CVector vecMoveSpeed = vecMoveDirection * damageIntensity;
            if (GetIsTypeVehicle() && vecMoveDirection.z < 0.7f)
            {
                vecMoveSpeed.z *= 0.3f;
            }

            if (!physicalFlags.bDisableCollisionForce)
            {
                bool bUpdateTurnSpeed = !GetIsTypeVehicle() || !CWorld::bNoMoreCollisionTorque;
                ApplyForce(vecMoveSpeed, vecDistanceToPoint, bUpdateTurnSpeed);
            }

            float fCollisionImpact1 = damageIntensity / fCollisionMass;
            AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            return true;
        }
    }
    return false;
}

// 0x543890
bool CPhysical::ApplySoftCollision(CEntity* entity, const CColPoint& colPoint, float& outDamageIntensity)
{
    if (physicalFlags.bDisableTurnForce)
    {
        ApplyCollision(entity, colPoint, outDamageIntensity);
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    CVector vecMoveDirection = colPoint.m_vecNormal;

    float fSoftColSpeedMult = SOFTCOL_SPEED_MULT;

    CVehicle* vehicle = this->AsVehicle();
    if (GetIsTypeVehicle() && vehicle->IsSubMonsterTruck())
    {
        float fForwardsDotProduct = DotProduct(&vecMoveDirection, &GetUp());
        if (fForwardsDotProduct < -0.9f)
        {
            return false;
        }

        if (fForwardsDotProduct < 0.0f)
        {
            vecMoveDirection -= fForwardsDotProduct * GetUp();
            vecMoveDirection.Normalise();
        }
        else if (fForwardsDotProduct > 0.5f)
        {
            fSoftColSpeedMult = SOFTCOL_SPEED_MULT2;
        }
    }

    float fSpeedDotProduct = DotProduct(&vecSpeed, &vecMoveDirection);
    CVector vecCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass)
    {
        vecCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    CVector vecDifference = vecDistanceToPointFromThis - vecCentreOfMassMultiplied;
    CVector vecSpeedCrossProduct = CrossProduct(vecDifference, vecMoveDirection);
    // Como el original: magnitud al cuadrado del producto cruz (el DLL usaba la de la normal)
    float fSquaredMagnitude = ApplyColSqMag(vecSpeedCrossProduct);
    float fCollisionMass = 1.0f / (fSquaredMagnitude / m_fTurnMass + 1.0f / m_fMass);

    if (!GetIsTypeVehicle() || vehicle->m_nVehicleSubType // todo: m_nVehicleSubType
        || colPoint.m_nPieceTypeA < 13u || colPoint.m_nPieceTypeA > 16u)
    {
        // Como el original: min < depth ? min : depth (con NaN toma depth)
        float fDepth = (SOFTCOL_DEPTH_MIN < colPoint.m_fDepth) ? SOFTCOL_DEPTH_MIN : colPoint.m_fDepth;

        outDamageIntensity = fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * 0.008f;
        if (!(fSpeedDotProduct < 0.0f)) // Como el original: NaN -> sin colisión
        {
            outDamageIntensity = 0.0f;
            return false;
        }
        outDamageIntensity -= fCollisionMass * fSpeedDotProduct * fSoftColSpeedMult;
    }
    else
    {
        outDamageIntensity = colPoint.m_fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * 0.008f;
        if (fSpeedDotProduct < 0.0f)
        {
            outDamageIntensity -= SOFTCOL_CARLINE_SPEED_MULT * fCollisionMass * fSpeedDotProduct;
        }

        float fRightDotProduct = DotProduct(&vecMoveDirection, &GetRight());
        vecMoveDirection -= 0.9f * fRightDotProduct * GetRight();
    }

    if (outDamageIntensity == 0.0f)
    {
        return false;
    }

    ApplyForce(vecMoveDirection * outDamageIntensity, vecDistanceToPointFromThis, true);
    if (outDamageIntensity < 0.0f)
    {
        outDamageIntensity *= -1.0f;
    }
    return true;
}

// 0x543C90
bool CPhysical::ApplySpringCollision(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, float& fSpringForceDampingLimit) {
    float fSpringStress = 1.0f - fSpringLength;
    if (fSpringStress <= 0.0f)
        return true;

    // Como el original: paso < 3.0 ? paso : 3.0 (NaN -> 3.0)
    const float fTimeStep = CTimer::GetTimeStep() < 3.0f ? CTimer::GetTimeStep() : 3.0f;

    fSpringForceDampingLimit = fSpringStress * m_fMass * fSuspensionForceLevel * 0.016f * fTimeStep * fSuspensionBias;
    // Como el original: fchs del límite (no multiplica por -1.0)
    ApplyForce((-fSpringForceDampingLimit) * direction, collisionPoint, true);
    return true;
}

// 0x543D60
bool CPhysical::ApplySpringCollisionAlt(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, CVector& normal, float& fSpringForceDampingLimit) {
    float fSpringStress = 1.0f - fSpringLength;
    if (fSpringStress <= 0.0f)
        return true;

    if (DotProduct(direction, normal) > 0.0f)
        normal *= -1.0f;

    // Como el original: paso < 3.0 ? paso : 3.0 (NaN -> 3.0)
    const float fTimeStep = CTimer::GetTimeStep() < 3.0f ? CTimer::GetTimeStep() : 3.0f;

    fSpringForceDampingLimit = fSpringStress * (fTimeStep * m_fMass) * fSuspensionForceLevel * fSuspensionBias * 0.016f;
    if (physicalFlags.bMakeMassTwiceAsBig)
        fSpringForceDampingLimit *= 0.75f;

    ApplyForce(fSpringForceDampingLimit * normal, collisionPoint, true);
    return true;
}

// 0x543E90
bool CPhysical::ApplySpringDampening(float fDampingForce, float fSpringForceDampingLimit, CVector& direction, CVector& collisionPoint, CVector& collisionPos) {
    float fCollisionPosDotProduct = DotProduct(collisionPos, direction);
    CVector vecCollisionPointSpeed = GetSpeed(collisionPoint);
    float fCollisionPointSpeedDotProduct = DotProduct(vecCollisionPointSpeed, direction);
    // Como el original: paso < 3.0 ? paso : 3.0 (NaN -> 3.0)
    const float fTimeStep = CTimer::GetTimeStep() < 3.0f ? CTimer::GetTimeStep() : 3.0f;

    float fDampingForceTimeStep = fTimeStep * fDampingForce;
    if (physicalFlags.bMakeMassTwiceAsBig)
        fDampingForceTimeStep *= 2.0f;

    // Como el original: primero > límite, después < -límite (NaN se conserva); sin std::clamp
    if (fDampingForceTimeStep > DAMPING_LIMIT_IN_FRAME)
        fDampingForceTimeStep = DAMPING_LIMIT_IN_FRAME;
    else if (fDampingForceTimeStep < -DAMPING_LIMIT_IN_FRAME)
        fDampingForceTimeStep = -DAMPING_LIMIT_IN_FRAME;
    float fDampingSpeed = -(fDampingForceTimeStep * fCollisionPosDotProduct);
    // Como el original: con velocidad NaN el resultado es 0
    if (fDampingSpeed > 0.0f && fDampingSpeed + fCollisionPointSpeedDotProduct > 0.0f) {
        fDampingSpeed = fCollisionPointSpeedDotProduct < 0.0f ? -fCollisionPointSpeedDotProduct : 0.0f;
    }
    else if (fDampingSpeed < 0.0f && fDampingSpeed + fCollisionPointSpeedDotProduct < 0.0f) {
        fDampingSpeed = fCollisionPointSpeedDotProduct > 0.0f ? -fCollisionPointSpeedDotProduct : 0.0f;
    }

    CVector center;
    center.FromMultiply3x3(*m_matrix, m_vecCentreOfMass); // Como el original: Multiply3x3 (0x59C790)
    CVector distance = collisionPoint - center;
    float fSpringForceDamping = GetMass(distance, direction) * fDampingSpeed;
    fSpringForceDampingLimit = fabs(fSpringForceDampingLimit) * DAMPING_LIMIT_OF_SPRING_FORCE;
    if (fSpringForceDamping > fSpringForceDampingLimit)
        fSpringForceDamping = fSpringForceDampingLimit;
    ApplyForce(fSpringForceDamping * direction, collisionPoint, true);
    return true;
}

// Unused
bool CPhysical::ApplySpringDampeningOld(float arg0, float arg1, CVector& arg2, CVector& arg3, CVector& arg4)
{
    return ((bool(__thiscall*)(CPhysical*, float, float, CVector&, CVector&, CVector&))0x544100)(this, arg0, arg1, arg2, arg3, arg4);
}

// 0x544280
void CPhysical::RemoveRefsToEntity(CEntity* entity)
{
    // Como el original: recorre desde 0 con índice int16 y compacta el array al encontrar la entidad
    if (!m_nNumEntitiesCollided)
        return;

    int16 collidedEntityIndex = 0;
    do {
        if (m_apCollidedEntities[collidedEntityIndex] == entity) {
            for (int16 i = collidedEntityIndex; i < m_nNumEntitiesCollided - 1; ++i) {
                m_apCollidedEntities[i] = m_apCollidedEntities[i + 1];
            }
            m_nNumEntitiesCollided--;
        }
        else {
            collidedEntityIndex++;
        }
    } while (collidedEntityIndex < m_nNumEntitiesCollided);
}

// 0x5442F0
void CPhysical::DettachEntityFromEntity(float x, float y, float z, bool bApplyTurnForce)
{
    CMatrix vecDetachOffsetMatrix{};
    if (GetIsTypeVehicle() && m_pAttachedTo && m_pAttachedTo->GetIsTypeVehicle()) {
        CColModel* colModel = CEntity::GetColModel();
        CCollisionData* colData = colModel->m_pColData;
        if (colData) {
            uint8 nNumLines = colData->m_nNumLines;
            colData->m_nNumLines = 0;
            CColModel* attachedToColModel = m_pAttachedTo->GetColModel();
            CMatrix& attachedToMatrix = m_pAttachedTo->GetMatrix();
            if (CCollision::ProcessColModels(*m_matrix, *colModel, attachedToMatrix, *attachedToColModel, CWorld::m_aTempColPts, nullptr, nullptr, false))
                m_pEntityIgnoredCollision = m_pAttachedTo;
            else if (m_pEntityIgnoredCollision == m_pAttachedTo)
                m_pEntityIgnoredCollision = nullptr;
            colData->m_nNumLines = nNumLines;
        }
        else if (m_pEntityIgnoredCollision == m_pAttachedTo) {
            m_pEntityIgnoredCollision = nullptr;
        }
    }
    else {
        m_pEntityIgnoredCollision = m_pAttachedTo;
    }

    vecDetachOffsetMatrix.ResetOrientation();
    vecDetachOffsetMatrix.RotateZ(y);
    vecDetachOffsetMatrix.RotateX(x);

    // Como el original: se copia la matriz del padre y se multiplica por la del desenganche
    // (padre * desenganche, 0x411A80), no al revés.
    CMatrix attachedToDetachMatrix;
    attachedToDetachMatrix = m_pAttachedTo->GetMatrix();
    attachedToDetachMatrix *= vecDetachOffsetMatrix;
    CVector vecForce = attachedToDetachMatrix.GetForward() * z;
    CWorld::Remove(this);
    SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    CWorld::Add(this);
    if (physicalFlags.bDisableCollisionForce) {
        CObjectData* pThisObjectInfo = AsObject()->m_pObjectInfo;
        if (!GetIsTypeObject() || !(pThisObjectInfo->m_fMass < 99998.0f)) // Como el original: NaN cuenta como masa infinita
        {
            physicalFlags.bCollidable = true;
            ResetTurnSpeed();
            ResetMoveSpeed();
            bApplyTurnForce = false;
        }
        else
        {
            physicalFlags.bDisableCollisionForce = false;
            m_fMass = pThisObjectInfo->m_fMass;
            m_fTurnMass = pThisObjectInfo->m_fTurnMass;
        }
    }

    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->GetIsTypePhysical()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecMoveSpeed += vecForce;
        }
    }

    if (bApplyTurnForce)
        ApplyTurnForce(vecForce, vecForce * 0.5f);

    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    m_vecAttachOffset = CVector(0.0f, 0.0f, 0.0f);
}

// 0x5446A0
void CPhysical::DettachAutoAttachedEntity()
{
    SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    m_nFakePhysics = 0;
    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->GetIsTypeVehicle()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    }
    else
    {
        physicalFlags.bCollidable = true;
        ResetTurnSpeed();
        ResetMoveSpeed();
    }
    m_vecAttachOffset = CVector(0.0f, 0.0f, 0.0f);
    m_pEntityIgnoredCollision = nullptr;
    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    if (GetIsTypeObject()) {
        m_fElasticity = AsObject()->m_pObjectInfo->m_fElasticity;
    }
}

// 0x5447B0
float CPhysical::GetLightingFromCol(bool bInteriorLighting)
{
    // Como el original: (azul + rojo) + verde, por la constante 0x863C10 (no es el float más cercano a 1/3)
    const float fBlue = CTimeCycle::GetAmbientBlue_BeforeBrightness();
    const float fRed = CTimeCycle::GetAmbientRed_BeforeBrightness();
    const float fGreen = CTimeCycle::GetAmbientGreen_BeforeBrightness();
    float fLighting = ((fBlue + fRed) + fGreen) * std::bit_cast<float>(0x3EAAAA9Fu) + m_fContactSurfaceBrightness;
    if (!bInteriorLighting) {
        // Como el original: luz * (rgb1 / 765 * frac + (1 - frac)) + rgb2 / 765 * frac
        fLighting = fLighting * (CTimeCycle::SumOfCurrentRGB1() * (1.0f / 765.0f) * TEST_ADD_AMBIENT_LIGHT_FRAC + (1.0f - TEST_ADD_AMBIENT_LIGHT_FRAC))
                    + CTimeCycle::SumOfCurrentRGB2() * (1.0f / 765.0f) * TEST_ADD_AMBIENT_LIGHT_FRAC;
    }
    return fLighting;
}

// 0x544850
float CPhysical::GetLightingTotal()
{
    bool bInteriorLighting = false;
    if (GetAreaCode())
        bInteriorLighting = true;

    if (GetIsTypePed()) {
        CPed* ped = AsPed();
        if (ped->GetPlayerData() && (CGame::currArea || ped->GetPlayerData()->m_bForceInteriorLighting))
            bInteriorLighting = true;
    }
    return GetLightingFromCol(bInteriorLighting) + m_fDynamicLighting;
}

// 0x5448B0
bool CPhysical::CanPhysicalBeDamaged(eWeaponType weapon, bool* bDamagedDueToFireOrExplosionOrBullet)
{
    if (bDamagedDueToFireOrExplosionOrBullet)
        *bDamagedDueToFireOrExplosionOrBullet = false;

    switch (weapon)
    {
    case WEAPON_UNARMED:
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_GOLFCLUB:
    case WEAPON_NIGHTSTICK:
    case WEAPON_KNIFE:
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL:
    case WEAPON_POOL_CUE:
    case WEAPON_KATANA:
    case WEAPON_CHAINSAW:
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2:
    case WEAPON_FLOWERS:
    case WEAPON_CANE:
        if (!physicalFlags.bMeleeProof)
            return true;
        return false;
    case WEAPON_GRENADE:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_DETONATOR:
    case WEAPON_ARMOUR | WEAPON_NIGHTSTICK:
        if (!physicalFlags.bExplosionProof)
            break;
        return false;
    case WEAPON_TEARGAS:
    case WEAPON_SPRAYCAN:
    case WEAPON_EXTINGUISHER:
    case WEAPON_CAMERA:
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
    case WEAPON_PARACHUTE:
    case WEAPON_LAST_WEAPON:
    case WEAPON_ARMOUR:
    case WEAPON_ARMOUR | WEAPON_BASEBALLBAT:
        return true;
    case WEAPON_MOLOTOV:
        // Como el original: el molotov también comprueba bExplosionProof
        if (physicalFlags.bExplosionProof || physicalFlags.bFireProof)
            return false;
        break;
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_TEC9:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
    case WEAPON_MINIGUN:
    case WEAPON_ARMOUR | WEAPON_KNIFE:
        if (physicalFlags.bBulletProof)
            return false;
        break;
    case WEAPON_FLAMETHROWER:
        if (!physicalFlags.bFireProof)
            return true;
        return false;
    case WEAPON_ARMOUR | WEAPON_BRASSKNUCKLE:
    case WEAPON_ARMOUR | WEAPON_GOLFCLUB:
    case WEAPON_ARMOUR | WEAPON_SHOVEL:
        if (physicalFlags.bCollisionProof)
            return false;
        return true;
    default:
        // Como el original: armas fuera de 0..54 devuelven true sin marcar el flag
        return true;
    }

    if (!bDamagedDueToFireOrExplosionOrBullet)
        return true;
    *bDamagedDueToFireOrExplosionOrBullet = true;
    return true;
}

// 0x544C40
void CPhysical::ApplyAirResistance()
{
    // Como el original: NaN o <= 0.1 (0x858B1C) va por la rama de la magnitud
    if (!(m_fAirResistance > 0.1f) || GetIsTypeVehicle())
    {
        float fSpeedMagnitude = m_vecMoveSpeed.Magnitude() * m_fAirResistance;
        if (CCullZones::DoExtraAirResistanceForPlayer())
        {
            if (GetIsTypeVehicle())
            {
                CVehicle* vehicle = this->AsVehicle();
                // Como el original: comprueba el subtipo (0x594) automóvil (0) o moto (9)
                if (vehicle->IsSubAutomobile() || vehicle->IsSubBike())
                    fSpeedMagnitude = CVehicle::m_fAirResistanceMult * fSpeedMagnitude;
            }
        }

        // Como el original: pow(1 - v, paso) con el _CIpow del EXE (0x822130) y ST0 completo en los productos
        const float fBase = 1.0f - fSpeedMagnitude;
        const float fStep = CTimer::GetTimeStep();
        float* const pMove = &m_vecMoveSpeed.x;
        __asm {
            fld fBase
            fld fStep
            mov eax, 0x822130
            call eax
            mov edx, pMove
            fld st(0)
            fmul dword ptr [edx]
            fstp dword ptr [edx]
            fld st(0)
            fmul dword ptr [edx+4]
            fstp dword ptr [edx+4]
            fmul dword ptr [edx+8]
            fstp dword ptr [edx+8]
        }
        m_vecTurnSpeed *= 0.99f;
    }
    else
    {
        // Como el original: pow(resistencia, paso) con el _CIpow del EXE (0x822130) y ST0 completo en los 6 productos
        const float fBase = m_fAirResistance;
        const float fStep = CTimer::GetTimeStep();
        float* const pMove = &m_vecMoveSpeed.x;
        float* const pTurn = &m_vecTurnSpeed.x;
        __asm {
            fld fBase
            fld fStep
            mov eax, 0x822130
            call eax
            mov edx, pMove
            fld st(0)
            fmul dword ptr [edx]
            fstp dword ptr [edx]
            fld st(0)
            fmul dword ptr [edx+4]
            fstp dword ptr [edx+4]
            fld st(0)
            fmul dword ptr [edx+8]
            fstp dword ptr [edx+8]
            mov edx, pTurn
            fld st(0)
            fmul dword ptr [edx]
            fstp dword ptr [edx]
            fld st(0)
            fmul dword ptr [edx+4]
            fstp dword ptr [edx+4]
            fmul dword ptr [edx+8]
            fstp dword ptr [edx+8]
        }
    }
}

// 0x544D50
bool CPhysical::ApplyCollisionAlt(CPhysical* entity, CColPoint& colPoint, float& damageIntensity, CVector& outVecMoveSpeed, CVector& outVecTurnSpeed)
{
    if (m_pAttachedTo)
    {
        if (m_pAttachedTo->GetIsTypePhysical()
            && !m_pAttachedTo->GetIsTypePed())
        {
            float fDamageIntensity = 0.0f;
            m_pAttachedTo->ApplySoftCollision(entity, colPoint, fDamageIntensity);
        }
    }

    if (physicalFlags.bDisableTurnForce)
    {
        float fSpeedDotProduct = DotProduct(&m_vecMoveSpeed, &colPoint.m_vecNormal);
        if (fSpeedDotProduct < 0.0f)
        {
            damageIntensity = -(fSpeedDotProduct * m_fMass);
            ApplyMoveForce(colPoint.m_vecNormal * damageIntensity);

            float fCollisionImpact1 = damageIntensity / m_fMass;
            AudioEngine.ReportCollision(
                this,
                entity,
                colPoint.m_nSurfaceTypeA,
                colPoint.m_nSurfaceTypeB,
                colPoint.m_vecPoint,
                &colPoint.m_vecNormal,
                fCollisionImpact1,
                1.0f,
                false,
                false
            );
            return true;
        }
        return false;
    }

    auto* vehicle = AsVehicle();
    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
        vecSpeed += vehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

    CVector vecMoveDirection = colPoint.m_vecNormal;
    // Como el original: (n.x*s.x + s.z*n.z) + n.y*s.y
    float fSpeedDotProduct = (vecMoveDirection.x * vecSpeed.x + vecSpeed.z * vecMoveDirection.z) + vecMoveDirection.y * vecSpeed.y;
    if (!(fSpeedDotProduct < 0.0f)) // Como el original: NaN -> false
    {
        return false;
    }

    CVector vecCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);
    if (physicalFlags.bInfiniteMass)
    {
        vecCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    CVector vecDifference = vecDistanceToPointFromThis - vecCentreOfMassMultiplied;
    CVector vecCrossProduct = CrossProduct(vecDifference, vecMoveDirection);
    float fSquaredMagnitude = vecCrossProduct.SquaredMagnitude();
    float fCollisionMass = 1.0f / (fSquaredMagnitude / m_fTurnMass + 1.0f / m_fMass);

    uint16 entityAltCol = ALT_ENITY_COL_DEFAULT;
    float fMoveSpeedLimit = CTimer::GetTimeStep() * 0.008f;
    if (GetIsTypeObject())
    {
        entityAltCol = ALT_ENITY_COL_OBJECT;
        fMoveSpeedLimit *= 1.3f;
    }
    else
    {
        if (GetIsTypeVehicle() && !physicalFlags.bSubmergedInWater) {
            float fMoveSpeedLimitMultiplier = 0.0f;
            if (!vehicle->IsBike() || (GetStatus() != STATUS_ABANDONED) && GetStatus() != STATUS_WRECKED)
            {
                if (vehicle->IsBoat())
                {
                    fMoveSpeedLimitMultiplier = 1.5f;
                    entityAltCol = ALT_ENITY_COL_BOAT;
                }
                else
                {
                    if (GetUp().z < -0.3f)
                    {
                        fMoveSpeedLimitMultiplier = 1.4f;
                        entityAltCol = ALT_ENITY_COL_VEHICLE;
                    }
                }
            }
            else
            {
                fMoveSpeedLimitMultiplier = 1.7f;
                entityAltCol = ALT_ENITY_COL_BIKE_WRECKED;
            }
            fMoveSpeedLimit *= fMoveSpeedLimitMultiplier;
        }
    }

    // todo: switch case should be used
    // 0x5450A1
    float fCollisionImpact2 = 1.0f;
    bool bUseElasticity = false;
    if (entityAltCol == ALT_ENITY_COL_OBJECT)
    {
        if (!GetHasContacted()
            && fabs(m_vecMoveSpeed.x) < fMoveSpeedLimit
            && fabs(m_vecMoveSpeed.y) < fMoveSpeedLimit
            && fMoveSpeedLimit + fMoveSpeedLimit > fabs(m_vecMoveSpeed.z))
        {
            fCollisionImpact2 = 0.0f;
            damageIntensity = -0.98f * fCollisionMass * fSpeedDotProduct;
        }
        bUseElasticity = true;
    }
    else
    {
        if (entityAltCol != ALT_ENITY_COL_BIKE_WRECKED)
        {
            if (entityAltCol == ALT_ENITY_COL_VEHICLE)
            {
                if (fabs(m_vecMoveSpeed.x) < fMoveSpeedLimit
                    && fabs(m_vecMoveSpeed.y) < fMoveSpeedLimit
                    && fMoveSpeedLimit + fMoveSpeedLimit > fabs(m_vecMoveSpeed.z))
                {
                    fCollisionImpact2 = 0.0f;
                    damageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
                }
                else
                {
                    bUseElasticity = true;
                }
            }
            else if (entityAltCol == ALT_ENITY_COL_BOAT
                && fabs(m_vecMoveSpeed.x) < fMoveSpeedLimit
                && fabs(m_vecMoveSpeed.y) < fMoveSpeedLimit
                && fMoveSpeedLimit + fMoveSpeedLimit > fabs(m_vecMoveSpeed.z))
            {
                fCollisionImpact2 = 0.0f;
                damageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
            }
            else
            {
                bUseElasticity = true;
            }
        }
        else
        {
            // Como el original: con NaN usa la elasticidad
            if (fabs(m_vecMoveSpeed.x) < fMoveSpeedLimit
                && fabs(m_vecMoveSpeed.y) < fMoveSpeedLimit
                && fMoveSpeedLimit + fMoveSpeedLimit > fabs(m_vecMoveSpeed.z))
            {
                fCollisionImpact2 = 0.0f;
                damageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
            }
            else
            {
                bUseElasticity = true;
            }
        }
    }

    if (bUseElasticity)
    {
        float fElasticity = m_fElasticity + m_fElasticity;
        // Como el original: doble elasticidad sólo si (madera || n.z < 0.5); con NaN la normal
        if (!GetIsTypeVehicle() || !vehicle->IsBoat()
            || colPoint.m_nSurfaceTypeB != SURFACE_WOOD_SOLID && !(vecMoveDirection.z < 0.5f))
        {
            fElasticity = m_fElasticity;
        }

        damageIntensity = -((fElasticity + 1.0f) * fCollisionMass * fSpeedDotProduct);
    }

    CVector vecMoveSpeed = vecMoveDirection * damageIntensity;

    if (physicalFlags.bDisableZ || physicalFlags.bInfiniteMass || physicalFlags.bDisableMoveForce)
    {
        ApplyForce(vecMoveSpeed, vecDistanceToPointFromThis, true);
    }
    else
    {
        CVector vecSpeed = ApplyColDivide(vecMoveSpeed, m_fMass);                                             // todo: shadow var
        if (GetIsTypeVehicle())
        {
            // Como el original: !(sqmag > 0.1) (con NaN también)
            if (!GetHasHitWall() || !(m_vecMoveSpeed.SquaredMagnitude() > 0.1f)
                && (entity->GetIsTypeBuilding() || entity->physicalFlags.bDisableCollisionForce))
            {
                outVecMoveSpeed += vecSpeed * 1.2f;
            }
            else
            {
                outVecMoveSpeed += vecSpeed;
            }

            vecMoveSpeed *= 0.8f;
        }
        else
        {
            outVecMoveSpeed += vecSpeed;
        }

        vecCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);
        float fTurnMass = m_fTurnMass;
        CVector vecDifference = vecDistanceToPointFromThis - vecCentreOfMassMultiplied; // todo: shadow var
        CVector vecCrossProduct = CrossProduct(vecDifference, vecMoveSpeed);            // todo: shadow var
        outVecTurnSpeed += ApplyColDivide(vecCrossProduct, fTurnMass);
    }

    float fCollisionImpact1 = damageIntensity / fCollisionMass;
    AudioEngine.ReportCollision(
        this,
        entity,
        colPoint.m_nSurfaceTypeA,
        colPoint.m_nSurfaceTypeB,
        colPoint.m_vecPoint,
        &colPoint.m_vecNormal,
        fCollisionImpact1,
        fCollisionImpact2,
        false,
        false
    );
    return true;
}

// 0x5454C0
bool CPhysical::ApplyFriction(float fFriction, CColPoint& colPoint)
{
    if (physicalFlags.bDisableCollisionForce)
    {
        return false;
    }

    if (physicalFlags.bDisableTurnForce)
    {
        float fMoveSpeedDotProduct = DotProduct(&m_vecMoveSpeed, &colPoint.m_vecNormal);
        CVector vecSpeedDifference = m_vecMoveSpeed - (fMoveSpeedDotProduct * colPoint.m_vecNormal);
        // Como el original: sqrt((z*z + y*y) + x*x)
        float fMoveSpeedMagnitude = std::sqrt((vecSpeedDifference.z * vecSpeedDifference.z + vecSpeedDifference.y * vecSpeedDifference.y) + vecSpeedDifference.x * vecSpeedDifference.x);
        if (fMoveSpeedMagnitude > 0.0f)
        {
            // Como el original: multiplica por el recíproco
            CVector vecMoveDirection = ApplyColDivide(vecSpeedDifference, fMoveSpeedMagnitude);

            float fSpeed = -fMoveSpeedMagnitude;
            float fForce = -(CTimer::GetTimeStep() / m_fMass * fFriction);
            if (fSpeed < fForce)
            {
                fSpeed = fForce;
            }

            m_vecFrictionMoveSpeed.x += vecMoveDirection.x * fSpeed;
            m_vecFrictionMoveSpeed.y += vecMoveDirection.y * fSpeed;
            return true;
        }
        return false;
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    float fMoveSpeedDotProduct = DotProduct(&vecSpeed, &colPoint.m_vecNormal);
    CVector vecSpeedDifference = vecSpeed - (fMoveSpeedDotProduct * colPoint.m_vecNormal);

    // Como el original: sqrt((z*z + y*y) + x*x)
    float fMoveSpeedMagnitude = std::sqrt((vecSpeedDifference.z * vecSpeedDifference.z + vecSpeedDifference.y * vecSpeedDifference.y) + vecSpeedDifference.x * vecSpeedDifference.x);
    if (!(fMoveSpeedMagnitude > 0.0f)) // Como el original: NaN -> false
    {
        return false;
    }

    // Como el original: multiplica por el recíproco
    CVector vecMoveDirection = ApplyColDivide(vecSpeedDifference, fMoveSpeedMagnitude);

    CVector vecCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);
    CVector vecDifference = vecDistanceToPointFromThis - vecCentreOfMassMultiplied;
    CVector vecSpeedCrossProduct = CrossProduct(vecDifference, vecMoveDirection);
    float squaredMagnitude = vecSpeedCrossProduct.SquaredMagnitude();
    float fCollisionMass = -(1.0f / (squaredMagnitude / m_fTurnMass + 1.0f / m_fMass) * fMoveSpeedMagnitude);
    float fNegativeFriction = -fFriction;
    if (fCollisionMass < fNegativeFriction)
    {
        fCollisionMass = fNegativeFriction;
    }

    ApplyFrictionForce(vecMoveDirection * fCollisionMass, vecDistanceToPointFromThis);

    auto* vehicle = AsVehicle();
    if (fMoveSpeedMagnitude > 0.1f
        && g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeB)
        && (g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeA) == FRICTION_EFFECT_SPARKS || GetIsTypeVehicle())
        && (!GetIsTypeVehicle() || !vehicle->IsSubBMX() || !vehicle->m_pDriver
            || !(fabs(DotProduct(&colPoint.m_vecNormal, &GetRight())) < 0.86669999f))) // Como el original: NaN -> chispas
    {

        CVector across = vecMoveDirection * (fMoveSpeedMagnitude * 0.25f);
        CVector direction = vecMoveDirection + (colPoint.m_vecNormal * 0.1f);
        CVector vecSpeedCrossProduct = CrossProduct(colPoint.m_vecNormal, m_vecMoveSpeed); // todo: shadow var
        vecSpeedCrossProduct.Normalise();

        for (int32 i = 0; i < 8; i++)
        {
            float fRandom = CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
            CVector origin = colPoint.m_vecPoint + (vecSpeedCrossProduct * fRandom);
            float force = fMoveSpeedMagnitude * 12.5f;
            g_fx.AddSparks(origin, direction, force, 1, across, SPARK_PARTICLE_SPARK2, 0.1f, 1.0f);
        }
    }
    return true;
}

// 0x545980
bool CPhysical::ApplyFriction(CPhysical* entity, float fFriction, CColPoint& colPoint)
{
    if (physicalFlags.bDisableTurnForce && entity->physicalFlags.bDisableTurnForce)
    {
        float fThisSpeedDotProduct = DotProduct(&m_vecMoveSpeed, &colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(&entity->m_vecMoveSpeed, &colPoint.m_vecNormal);

        CVector vecThisSpeedDifference = m_vecMoveSpeed - (fThisSpeedDotProduct * colPoint.m_vecNormal);
        CVector vecEntitySpeedDifference = entity->m_vecMoveSpeed - (fEntitySpeedDotProduct * colPoint.m_vecNormal);

        float fThisSpeedMagnitude = ApplyColMagXZY(vecThisSpeedDifference);
        float fEntitySpeedMagnitude = ApplyColMagXZY(vecEntitySpeedDifference);

        float fEntityMass = entity->m_fMass;
        float fThisMass = m_fMass;

        // Como el original: multiplica por el recíproco
        CVector vecMoveDirection = ApplyColDivide(vecThisSpeedDifference, fThisSpeedMagnitude);
        float fSpeed = (fEntityMass * fEntitySpeedMagnitude + fThisMass * fThisSpeedMagnitude) / (fEntityMass + fThisMass);
        if (fThisSpeedMagnitude > fSpeed) {
            float fThisSpeed = fThisMass * (fSpeed - fThisSpeedMagnitude);
            float fEntitySpeed = fEntityMass * (fSpeed - fEntitySpeedMagnitude);
            float fFrictionTimeStep = -(CTimer::GetTimeStep() * fFriction);
            if (fThisSpeed < fFrictionTimeStep)
            {
                fThisSpeed = fFrictionTimeStep;
            }

            // BUG: Both if conditions are the same.
            if (fThisSpeed < fFrictionTimeStep)
            {
                fThisSpeed = fFrictionTimeStep;
            }

            ApplyFrictionMoveForce(vecMoveDirection * fThisSpeed);
            entity->ApplyFrictionMoveForce(vecMoveDirection * fEntitySpeed);
            return true;
        }
        return false;
    }

    if (physicalFlags.bDisableTurnForce)
    {
        if (entity->GetIsTypeVehicle())
        {
            return false;
        }

        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);

        float fThisSpeedDotProduct = DotProduct(&m_vecMoveSpeed, &colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(&vecEntitySpeed, &colPoint.m_vecNormal);

        CVector vecThisSpeedDifference = m_vecMoveSpeed - (fThisSpeedDotProduct * colPoint.m_vecNormal);
        CVector vecEntitySpeedDifference = vecEntitySpeed - (fEntitySpeedDotProduct * colPoint.m_vecNormal);

        float fThisSpeedMagnitude = ApplyColMagZYX(vecThisSpeedDifference);
        float fEntitySpeedMagnitude = ApplyColMagZYX(vecEntitySpeedDifference);

        CVector vecMoveDirection = vecThisSpeedDifference * (1.0f / fThisSpeedMagnitude);
        CVector vecEntityCentreOfMassMultiplied = ApplyColMultiply3x3(entity->GetMatrix(), entity->m_vecCentreOfMass);
        CVector vecEntityDifference = vecDistanceToPoint - vecEntityCentreOfMassMultiplied;
        CVector vecEntitySpeedCrossProduct = CrossProduct(vecEntityDifference, vecMoveDirection);
        float squaredMagnitude = vecEntitySpeedCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = 1.0f / ((squaredMagnitude) / entity->m_fTurnMass + 1.0f / entity->m_fMass);
        float fThisMass = m_fMass;
        float fSpeed = (fEntitySpeedMagnitude * fEntityCollisionMass + fThisMass * fThisSpeedMagnitude) / (fEntityCollisionMass + fThisMass);
        if (fThisSpeedMagnitude > fSpeed) {
            float fThisSpeed = fThisMass * (fSpeed - fThisSpeedMagnitude);
            float fEntitySpeed = fEntityCollisionMass * (fSpeed - fEntitySpeedMagnitude);
            float fFrictionTimeStep = CTimer::GetTimeStep() * fFriction;
            float fFrictionTimeStepNegative = -fFrictionTimeStep;
            if (fThisSpeed < fFrictionTimeStepNegative)
            {
                fThisSpeed = fFrictionTimeStepNegative;
            }

            if (fEntitySpeed > fFrictionTimeStep)
            {
                fEntitySpeed = fFrictionTimeStep;
            }

            ApplyFrictionMoveForce(vecMoveDirection * fThisSpeed);
            if (!entity->physicalFlags.bDisableCollisionForce)
            {
                entity->ApplyFrictionForce(vecMoveDirection * fEntitySpeed, vecDistanceToPoint);
                return true;
            }
            return true;
        }
        return false;
    }

    if (!entity->physicalFlags.bDisableTurnForce)
    {
        CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
        CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);

        float fThisSpeedDotProduct = DotProduct(&vecThisSpeed, &colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(&vecEntitySpeed, &colPoint.m_vecNormal);
        // Como el original: las constantes se leen de variables globales (0x8CD7CC * 0x8CD7C8, 0x8CD7C4)
        if (StaticRef<float>(0x8CD7CC) * StaticRef<float>(0x8CD7C8) > fabs(fThisSpeedDotProduct))
        {
            fFriction = StaticRef<float>(0x8CD7C4) * fFriction;
        }

        CVector vecThisSpeedDifference = vecThisSpeed - (fThisSpeedDotProduct * colPoint.m_vecNormal);
        CVector vecEntitySpeedDifference = vecEntitySpeed - (fEntitySpeedDotProduct * colPoint.m_vecNormal);

        float fThisSpeedMagnitude = ApplyColMagXZY(vecThisSpeedDifference);
        float fEntitySpeedMagnitude = ApplyColMagXZY(vecEntitySpeedDifference);

        CVector vecMoveDirection = vecThisSpeedDifference * (1.0f / fThisSpeedMagnitude);

        CVector vecThisCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);

        CVector vecThisDifference = vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied;
        CVector vecThisSpeedCrossProduct = CrossProduct(vecThisDifference, vecMoveDirection);
        float squaredMagnitude = vecThisSpeedCrossProduct.SquaredMagnitude();
        float fThisCollisionMass = 1.0f / (squaredMagnitude / m_fTurnMass + 1.0f / m_fMass);

        CVector vecEntityCentreOfMassMultiplied = ApplyColMultiply3x3(entity->GetMatrix(), entity->m_vecCentreOfMass);

        CVector vecEntityDifference = vecDistanceToPoint - vecEntityCentreOfMassMultiplied;
        CVector vecEntitySpeedCrossProduct = CrossProduct(vecEntityDifference, vecMoveDirection);
        squaredMagnitude = vecEntitySpeedCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = 1.0f / (squaredMagnitude / entity->m_fTurnMass + 1.0f / entity->m_fMass);
        float fSpeed = (fEntitySpeedMagnitude * fEntityCollisionMass + fThisCollisionMass * fThisSpeedMagnitude) / (fEntityCollisionMass + fThisCollisionMass);
        if (fThisSpeedMagnitude > fSpeed) {
            float fThisSpeed = fThisCollisionMass * (fSpeed - fThisSpeedMagnitude);
            float fEntitySpeed = fEntityCollisionMass * (fSpeed - fEntitySpeedMagnitude);
            float fNegativeFriction = -fFriction;
            if (fThisSpeed < fNegativeFriction) {
                fThisSpeed = fNegativeFriction;
            }

            if (fEntitySpeed > fFriction) {
                fEntitySpeed = fFriction;
            }

            if (!physicalFlags.bDisableCollisionForce) {
                ApplyFrictionForce(vecMoveDirection * fThisSpeed, vecDistanceToPointFromThis);
            }

            if (!entity->physicalFlags.bDisableCollisionForce) {
                entity->ApplyFrictionForce(vecMoveDirection * fEntitySpeed, vecDistanceToPoint);
            }
            return true;
        }
        return false;
    }

    if (GetIsTypeVehicle())
    {
        return false;
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

    float fThisSpeedDotProduct = DotProduct(&vecThisSpeed, &colPoint.m_vecNormal);
    float fEntitySpeedDotProduct = DotProduct(&entity->m_vecMoveSpeed, &colPoint.m_vecNormal);

    CVector vecThisSpeedDifference = vecThisSpeed - (fThisSpeedDotProduct * colPoint.m_vecNormal);
    CVector vecEntitySpeedDifference = entity->m_vecMoveSpeed - (fEntitySpeedDotProduct * colPoint.m_vecNormal);

    float fThisSpeedMagnitude = ApplyColMagXZY(vecThisSpeedDifference);
    float fEntitySpeedMagnitude = ApplyColMagXZY(vecEntitySpeedDifference);

    CVector vecMoveDirection = vecThisSpeedDifference * (1.0f / fThisSpeedMagnitude);

    CVector vecThisCentreOfMassMultiplied = ApplyColMultiply3x3(GetMatrix(), m_vecCentreOfMass);

    CVector vecThisDifference = vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied;
    CVector vecThisSpeedCrossProduct = CrossProduct(vecThisDifference, vecMoveDirection);
    float squaredMagnitude = vecThisSpeedCrossProduct.SquaredMagnitude();
    float fEntityMass = entity->m_fMass;
    float fThisCollisionMass = 1.0f / (squaredMagnitude / m_fTurnMass + 1.0f / m_fMass);
    float fSpeed = (fEntityMass * fEntitySpeedMagnitude + fThisCollisionMass * fThisSpeedMagnitude) / (fEntityMass + fThisCollisionMass);
    if (fThisSpeedMagnitude > fSpeed) {
        float fThisSpeed = (fSpeed - fThisSpeedMagnitude) * fThisCollisionMass;
        float fEntitySpeed = (fSpeed - fEntitySpeedMagnitude) * fEntityMass;
        float fFrictionTimeStep = CTimer::GetTimeStep() * fFriction;
        float fNegativeFrictionTimeStep = -fFrictionTimeStep;
        if (fThisSpeed < fNegativeFrictionTimeStep) {
            fThisSpeed = fNegativeFrictionTimeStep;
        }

        if (fEntitySpeed > fFrictionTimeStep) {
            fEntitySpeed = fFrictionTimeStep;
        }

        if (!physicalFlags.bDisableCollisionForce) {
            ApplyFrictionForce(vecMoveDirection * fThisSpeed, vecDistanceToPointFromThis);
        }

        entity->ApplyFrictionMoveForce(vecMoveDirection * fEntitySpeed);
        return true;
    }
    return false;
}

// 0x546670
bool CPhysical::ProcessShiftSectorList(int32 sectorX, int32 sectorY)
{
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(m_nModelIndex);
    float fBoundingSphereRadius = mi->GetColModel()->GetBoundRadius();
    float fMaxColPointDepth = 0.0f;
    CVector vecShift{};
    CColPoint colPoints[32];
    CVector vecBoundCentre = GetBoundCentre();

    int32 totalAcceptableColPoints = 0;
    // Como el original: estos dos flags solo se ponen a 0 al entrar en la función, no por entidad
    bool bCollidedEntityUnableToMove = false;
    bool bThisOrCollidedEntityStuck = false;
    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) {
        for (CEntity* const entity : list) {
            // Como el original: se recalcula para cada entidad (edificio, objeto sin fuerza de colisión,
            // u objeto estático/esperando colisión no explotado si somos un ped)
            bool bProcessEntityCollision;
            if (entity->GetIsTypeBuilding() || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)) {
                bProcessEntityCollision = true;
            } else {
                bProcessEntityCollision = GetIsTypePed() && entity->GetIsTypeObject() && entity->GetIsStatic() && !entity->AsObject()->objectFlags.bIsExploded;
            }
            if (entity != this
                && !entity->IsScanCodeCurrent()
                && entity->GetUsesCollision() && (!GetHasHitWall() || bProcessEntityCollision))
            {
                if (entity->GetIsTouching(vecBoundCentre, fBoundingSphereRadius))
                {
                    bool bCollisionDisabled = false;
                    bool bCollidedEntityCollisionIgnored = false;
                    if (entity->GetIsTypeBuilding())
                    {
                        // Como el original: se comprueba si ESTE vehículo es un tren (no la entidad colisionada)
                        if (physicalFlags.bDisableCollisionForce
                            && (!GetIsTypeVehicle() || AsVehicle()->IsSubTrain()))
                        {
                            bCollisionDisabled = true;
                        }
                        // Como el original: si m_pAttachedTo no es un físico sin fuerza de colisión se siguen evaluando las demás condiciones
                        else if (m_pAttachedTo && m_pAttachedTo->GetIsTypePhysical() && m_pAttachedTo->physicalFlags.bDisableCollisionForce)
                        {
                            bCollisionDisabled = true;
                        }
                        else if (m_pEntityIgnoredCollision == entity)
                        {
                            bCollisionDisabled = true;
                        }
                        else if (physicalFlags.bDisableZ && !physicalFlags.bApplyGravity)
                        {
                            bCollisionDisabled = true;
                        }
                        else if (physicalFlags.bDontCollideWithFlyers
                            && GetStatus() != STATUS_PLAYER
                            && GetStatus() != STATUS_REMOTE_CONTROLLED
                            && entity->DoesNotCollideWithFlyers()
                        ) {
                            bCollisionDisabled = true;
                        }
                    }
                    else
                    {
                        SpecialEntityPreCollisionStuff(
                            entity->AsPhysical(),
                            true,
                            bCollisionDisabled,
                            bCollidedEntityCollisionIgnored,
                            bCollidedEntityUnableToMove,
                            bThisOrCollidedEntityStuck
                        );
                    }

                    if (GetIsTypePed())
                    {
                        physicalFlags.bSkipLineCol = true;
                    }

                    if (bCollidedEntityCollisionIgnored || bCollisionDisabled)
                        continue;

                    entity->SetCurrentScanCode();
                    int32 totalColPointsToProcess = ProcessEntityCollision(entity, colPoints);
                    if (totalColPointsToProcess <= 0)
                        continue;

                    for (int32 colpointIndex = 0; colpointIndex < totalColPointsToProcess; colpointIndex++) {
                        CColPoint* colPoint = &colPoints[colpointIndex];
                        if (colPoint->m_fDepth > 0.0f) {
                            uint8 pieceTypeB = colPoint->m_nPieceTypeB;
                            if (pieceTypeB < 13 || pieceTypeB > 16) {
                                totalAcceptableColPoints++;
                                if (GetIsTypeVehicle() && entity->GetIsTypePed() && colPoint->m_vecNormal.z < 0.0f) {
                                    vecShift.x += colPoint->m_vecNormal.x;
                                    vecShift.y += colPoint->m_vecNormal.y;
                                    vecShift.z += colPoint->m_vecNormal.z * 0.0f;
                                    fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                } else {
                                    // Como el original: solo fabs(z) > 0.1 descarta el punto (NaN no lo descarta)
                                    if (!GetIsTypePed() || !entity->GetIsTypeObject() || !entity->AsPhysical()->physicalFlags.bDisableMoveForce ||
                                        !(fabs(colPoint->m_vecNormal.z) > 0.1f)) {
                                        vecShift += colPoint->m_vecNormal;
                                        fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    // Como el original: el sector se limita a [0, 119] y el sector repetido usa `& 15` (también con coordenadas negativas)
    auto& s = CWorld::GetSector(sectorX, sectorY);
    auto& rs = CWorld::ms_aRepeatSectors[(uint32)sectorY & (MAX_REPEAT_SECTORS_Y - 1)][(uint32)sectorX & (MAX_REPEAT_SECTORS_X - 1)];
    // Como el original: las listas se recorren en orden objetos, peds, vehículos, edificios
    ProcessSectorList(rs.Objects);
    ProcessSectorList(rs.Peds);
    ProcessSectorList(rs.Vehicles);
    ProcessSectorList(s.Buildings);

    if (totalAcceptableColPoints == 0) {
        return false;
    }

    // Como el original: magnitud sumada como (x*x + z*z) + y*y
    float shiftMagnitude = std::sqrt((vecShift.x * vecShift.x + vecShift.z * vecShift.z) + vecShift.y * vecShift.y);
    if (shiftMagnitude > 1.0f) { // normalize the shift boi
        float shiftMultiplier = 1.0f / shiftMagnitude;
        vecShift.x *= shiftMultiplier;
        vecShift.y *= shiftMultiplier;
        vecShift.z *= shiftMultiplier;
    }

    CVector& vecEntityPosition = GetPosition();
    CVector vecPosShift;
    // Como el original: con z NaN no se toma la rama de 0.75
    if (vecShift.z < -0.5f) {
        vecPosShift = vecShift * fMaxColPointDepth * 0.75f;
    } else if (GetIsTypePed()) {
        // Como el original: el desplazamiento del ped es vecShift * clamp(1.5 * profundidad, 0.005, 0.3)
        const float fDepthMultiplied = 1.5f * fMaxColPointDepth;
        float fShiftScale;
        if (fDepthMultiplied < 0.005f) {
            fShiftScale = 0.005f;
        } else if (0.3f < fDepthMultiplied) {
            fShiftScale = 0.3f;
        } else {
            fShiftScale = fDepthMultiplied;
        }
        vecPosShift = vecShift * fShiftScale;
    } else {
        vecPosShift = vecShift * fMaxColPointDepth * 1.5f;
    }
    vecEntityPosition.x = vecPosShift.x + vecEntityPosition.x;
    vecEntityPosition.y = vecPosShift.y + vecEntityPosition.y;
    vecEntityPosition.z = vecPosShift.z + vecEntityPosition.z;

    if (!GetIsTypeVehicle() || 1.5f <= 0.0f) { // R* nice check
        return true;
    }

    if (vecShift.z < 0.0f) {
        vecShift.z = 0.0f;
    }

    m_vecMoveSpeed += vecShift * 0.008f * CTimer::GetTimeStep();
    return true;
}

namespace {
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del exe
CVector PhysAttachMatPoint59C890(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z
    };
}

// Como el original: Multiply3x3 (0x59C790) con el orden de sumas del exe
CVector PhysAttachMat3x3_59C790(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y
    };
}

// Como el original: transcripción exacta de CMatrix::Rotate(x, y, z) (0x59B460), con fsin/fcos del x87
// y su orden de productos (la versión DLL de CMatrix::Rotate(CVector) usa otro orden).
__declspec(naked) void __fastcall PhysAttachRotate59B460(CMatrix* /*ecx*/, int /*edx*/, float /*x*/, float /*y*/, float /*z*/) {
    __asm {
        sub esp,54h
        fld dword ptr [esp+58h]
        fcos
        fld dword ptr [esp+58h]
        fsin
        fstp dword ptr [esp+58h]
        fld dword ptr [esp+5Ch]
        fcos
        fld dword ptr [esp+5Ch]
        fsin
        fld dword ptr [esp+60h]
        fcos
        fld dword ptr [esp+60h]
        fsin
        fst dword ptr [esp+5Ch]
        fmul dword ptr [esp+58h]
        fld st(1)
        fmul dword ptr [esp+58h]
        fstp dword ptr [esp+60h]
        fld st(1)
        fmul st,st(4)
        fld st(1)
        fmul st,st(4)
        fsubp st(1),st
        fstp dword ptr [esp]
        fld dword ptr [esp+60h]
        fmul st,st(3)
        fld dword ptr [esp+5Ch]
        fmul st,st(5)
        faddp st(1),st
        fstp dword ptr [esp+4]
        fld st(2)
        fmul st,st(5)
        fchs
        fstp dword ptr [esp+8]
        fld dword ptr [esp+5Ch]
        fmul st,st(5)
        fchs
        fstp dword ptr [esp+0Ch]
        fld st(1)
        fmul st,st(5)
        fstp dword ptr [esp+10h]
        fxch st(1)
        fmul st,st(2)
        fxch st(1)
        fmul st,st(3)
        faddp st(1),st
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+5Ch]
        fmul st,st(1)
        fld dword ptr [esp+60h]
        fmul st,st(3)
        fsubp st(1),st
        fstp dword ptr [esp+1Ch]
        fstp st(0)
        fmulp st(1),st
        fld dword ptr [esp+18h]
        fmul dword ptr [ecx+8]
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [esp]
        fmul dword ptr [ecx]
        faddp st(1),st
        fld dword ptr [esp+1Ch]
        fmul dword ptr [ecx+8]
        fld dword ptr [esp+10h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [esp+4]
        fmul dword ptr [ecx]
        faddp st(1),st
        fld st(2)
        fmul dword ptr [ecx+8]
        fld dword ptr [esp+58h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [esp+8]
        fmul dword ptr [ecx]
        faddp st(1),st
        fstp dword ptr [esp+2Ch]
        fld dword ptr [esp]
        fmul dword ptr [ecx+10h]
        fld dword ptr [esp+18h]
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fstp dword ptr [esp+30h]
        fld dword ptr [esp+4]
        fmul dword ptr [ecx+10h]
        fld dword ptr [esp+1Ch]
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fld dword ptr [esp+10h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fstp dword ptr [esp+34h]
        fld dword ptr [esp+8]
        fmul dword ptr [ecx+10h]
        fld st(3)
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fld dword ptr [esp+58h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fstp dword ptr [esp+38h]
        fld dword ptr [esp]
        fmul dword ptr [ecx+20h]
        fld dword ptr [esp+18h]
        fmul dword ptr [ecx+28h]
        faddp st(1),st
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+24h]
        faddp st(1),st
        fstp dword ptr [esp+3Ch]
        fld dword ptr [esp+4]
        fmul dword ptr [ecx+20h]
        fld dword ptr [esp+1Ch]
        fmul dword ptr [ecx+28h]
        faddp st(1),st
        fld dword ptr [esp+10h]
        fmul dword ptr [ecx+24h]
        faddp st(1),st
        fstp dword ptr [esp+40h]
        fld dword ptr [esp+8]
        fmul dword ptr [ecx+20h]
        fld st(3)
        fmul dword ptr [ecx+28h]
        faddp st(1),st
        fld dword ptr [esp+58h]
        fmul dword ptr [ecx+24h]
        faddp st(1),st
        fstp dword ptr [esp+44h]
        fld dword ptr [esp]
        fmul dword ptr [ecx+30h]
        fld dword ptr [esp+18h]
        fmul dword ptr [ecx+38h]
        faddp st(1),st
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+34h]
        faddp st(1),st
        fstp dword ptr [esp+48h]
        fld dword ptr [esp+4]
        fmul dword ptr [ecx+30h]
        fld dword ptr [esp+1Ch]
        fmul dword ptr [ecx+38h]
        faddp st(1),st
        fld dword ptr [esp+10h]
        fmul dword ptr [ecx+34h]
        faddp st(1),st
        fstp dword ptr [esp+4Ch]
        fld dword ptr [esp+8]
        fmul dword ptr [ecx+30h]
        fxch st(3)
        fmul dword ptr [ecx+38h]
        mov eax,dword ptr [esp+2Ch]
        faddp st(3),st
        fld dword ptr [esp+58h]
        mov edx,dword ptr [esp+30h]
        fmul dword ptr [ecx+34h]
        mov dword ptr [ecx+8],eax
        mov eax,dword ptr [esp+34h]
        mov dword ptr [ecx+10h],edx
        mov edx,dword ptr [esp+38h]
        faddp st(3),st
        mov dword ptr [ecx+14h],eax
        fxch st(2)
        mov eax,dword ptr [esp+3Ch]
        mov dword ptr [ecx+18h],edx
        fstp dword ptr [esp+50h]
        mov edx,dword ptr [esp+40h]
        mov dword ptr [ecx+20h],eax
        fstp dword ptr [ecx]
        mov eax,dword ptr [esp+44h]
        mov dword ptr [ecx+24h],edx
        fstp dword ptr [ecx+4]
        mov edx,dword ptr [esp+48h]
        mov dword ptr [ecx+28h],eax
        mov eax,dword ptr [esp+4Ch]
        mov dword ptr [ecx+30h],edx
        mov edx,dword ptr [esp+50h]
        mov dword ptr [ecx+34h],eax
        mov dword ptr [ecx+38h],edx
        add esp,54h
        ret 0Ch
    }
}
}

// Used in driving school mission
// 0x546DB0
void CPhysical::PlacePhysicalRelativeToOtherPhysical(CPhysical* relativeToPhysical, CPhysical* physicalToPlace, CVector offset)
{
    // Como el original: 0x59C890 con su orden de sumas
    CVector vecRelativePosition = PhysAttachMatPoint59C890(*relativeToPhysical->m_matrix, offset);
    vecRelativePosition += (CTimer::GetTimeStep() * 0.9f) * relativeToPhysical->m_vecMoveSpeed;
    CWorld::Remove(physicalToPlace);
    *(CMatrix*)physicalToPlace->m_matrix = *relativeToPhysical->m_matrix;
    physicalToPlace->GetPosition() = vecRelativePosition;
    physicalToPlace->m_vecMoveSpeed = relativeToPhysical->m_vecMoveSpeed;
    physicalToPlace->UpdateRwMatrix();
    physicalToPlace->UpdateRwFrame();
    CWorld::Add(physicalToPlace);
}

// Unused
// 0x546ED0
float CPhysical::ApplyScriptCollision(CVector arg0, float arg1, float arg2, CVector* arg3)
{
    return ((float(__thiscall*)(CPhysical*, CVector, float, float, CVector*))0x546ED0)(this, arg0, arg1, arg2, arg3);
}

// 0x546FF0
void CPhysical::PositionAttachedEntity()
{
    if (!m_pAttachedTo)
        return;

    auto* attachedToVehicle = m_pAttachedTo->AsVehicle();
    auto* attachedToAuto = m_pAttachedTo->AsAutomobile();
    auto* attachedToBike = m_pAttachedTo->AsBike();

    CMatrix attachedEntityMatrix;
    CMatrix attachedEntityRotationMatrix;
    CMatrix attachedToEntityMatrix (m_pAttachedTo->GetMatrix());
    if (m_pAttachedTo->GetIsTypeVehicle() && attachedToVehicle->IsBike()) {
        attachedToBike->CalculateLeanMatrix();
        attachedToEntityMatrix = attachedToBike->m_mLeanMatrix;
    }
    else {
        if (GetIsTypeObject() && m_pAttachedTo->m_nModelIndex == MODEL_FORKLIFT) {
            RwFrame* carPart = attachedToAuto->m_aCarNodes[CAR_MISC_A];
            if (carPart)
                attachedToEntityMatrix.UpdateMatrix(RwFrameGetLTM(carPart));
        }
    }

    // Como el original: solo se comprueba la parte imaginaria (NaN cuenta como distinta de cero)
    const CVector& quatImag = m_qAttachedEntityRotation.imag;
    if (quatImag.x != 0.0f || quatImag.y != 0.0f || quatImag.z != 0.0f) {
        // Como el original: conversión cuaternión -> matriz en línea con el orden x87 del exe
        // (RtQuatConvertToMatrix no se llama); pos = 0.
        const float qx = m_qAttachedEntityRotation.imag.x;
        const float qy = m_qAttachedEntityRotation.imag.y;
        const float qz = m_qAttachedEntityRotation.imag.z;
        const float qw = m_qAttachedEntityRotation.real;
        const float s  = 2.0f / (((qx * qx + qy * qy) + qz * qz) + qw * qw);
        const float sx = s * qx, sy = s * qy, sz = s * qz;
        const float wx = sx * qw, wy = sy * qw, wz = sz * qw;
        const float xx = sx * qx, yy = sy * qy, zz = sz * qz;
        const float yz = sz * qy, xz = sx * qz, xy = sy * qx;
        RwMatrix rotationMatrix;
        rotationMatrix.right = { 1.0f - (zz + yy), wz + xy, xz - wy };
        rotationMatrix.flags = 3;
        rotationMatrix.up    = { xy - wz, 1.0f - (zz + xx), yz + wx };
        rotationMatrix.at    = { xz + wy, yz - wx, 1.0f - (yy + xx) };
        rotationMatrix.pos   = { 0.0f, 0.0f, 0.0f };
        attachedEntityMatrix.UpdateMatrix(&rotationMatrix);
        attachedEntityMatrix = attachedToEntityMatrix * attachedEntityMatrix;
        CVector vecTranslation = PhysAttachMatPoint59C890(attachedToEntityMatrix, m_vecAttachOffset);
        attachedEntityMatrix.SetTranslateOnly(vecTranslation);
    }
    else {
        if (physicalFlags.bInfiniteMass) {
            attachedEntityMatrix = *m_matrix;
        }
        else {
            attachedEntityRotationMatrix.ResetOrientation();
            PhysAttachRotate59B460(&attachedEntityRotationMatrix, 0, m_vecAttachedEntityRotation.x, m_vecAttachedEntityRotation.y, m_vecAttachedEntityRotation.z);
            attachedEntityMatrix = attachedToEntityMatrix;
            attachedEntityMatrix *= attachedEntityRotationMatrix;
        }
        // Como el original: Multiply3x3 (0x59C790) + posición
        const CVector vecOffset = PhysAttachMat3x3_59C790(attachedToEntityMatrix, m_vecAttachOffset);
        const CVector& vecAttachedToPos = attachedToEntityMatrix.GetPosition();
        attachedEntityMatrix.GetPosition() = CVector{ vecAttachedToPos.x + vecOffset.x, vecAttachedToPos.y + vecOffset.y, vecAttachedToPos.z + vecOffset.z };
    }
    SetMatrix(attachedEntityMatrix);

    if (GetIsTypeObject()) {
        if (GetIsStatic())
            SetIsStatic(false);
        physicalFlags.bAttachedToEntity = true;
        m_nFakePhysics = 0;
    }

    bool bUpdateSpeed = false;
    if (!m_pAttachedTo->GetIsTypeVehicle() || !this->GetIsTypeObject() && !this->GetIsTypeVehicle()) {
        if (m_nModelIndex == MODEL_SATCHEL && m_pAttachedTo->GetIsTypeObject() && !m_pAttachedTo->GetUsesCollision()) {
            DettachAutoAttachedEntity();
            SetUsesCollision(true);
            return;
        }
        bUpdateSpeed = true;
    }

    bool bDettachEntity = false;
    if (!bUpdateSpeed)
    {
        if (m_pAttachedTo->m_nModelIndex == MODEL_DUMPER) {
            uint16 wMiscComponentAngle = attachedToAuto->m_wMiscComponentAngle; // Como el original: comparación de words
            if (wMiscComponentAngle && wMiscComponentAngle != attachedToAuto->m_wMiscComponentAnglePrev) {
                bDettachEntity = true;
            }
            else if (m_fDamageIntensity > 0.0f) {
                if (m_pDamageEntity) {
                    if (m_pDamageEntity->GetIsTypeBuilding())
                        bDettachEntity = true;
                }
            }
        }
        else {
            if (m_pAttachedTo->m_nModelIndex != MODEL_FORKLIFT) {
                if (GetIsTypeVehicle()) {
                    const CMatrix& attachedToMatrix = m_pAttachedTo->GetMatrix();
                    if (fabs(attachedToMatrix.GetRight().z) > 0.707f || fabs(attachedToMatrix.GetForward().z) > 0.707f) { // 0.707 == sin(DegreesToRadians(45))
                        DettachEntityFromEntity(0.0f, 0.0f, 1.0f, false);
                        return;
                    }
                }
                bUpdateSpeed = true;
            }
            else if (!attachedToAuto->m_wMiscComponentAngle && attachedToAuto->m_wMiscComponentAnglePrev
                || m_fDamageIntensity > 0.1f * m_fMass && m_pDamageEntity && m_pDamageEntity->GetIsTypeBuilding()) {
                bDettachEntity = true;
            }
        }
        if (!bUpdateSpeed && !bDettachEntity) {
            // Como el original: las comparaciones con NaN no provocan el desenganche
            float fDamagedIntensity = m_pAttachedTo->m_fDamageIntensity;
            CMatrix* matrix = m_pAttachedTo->m_matrix;
            if (fDamagedIntensity > 0.0f && matrix->GetUp().z < 0.1f
                || fDamagedIntensity > 1.0f && matrix->GetForward().z > 0.87f) {
                bDettachEntity = true;
            } else if (fDamagedIntensity > 500.0f
                && DotProduct(&m_pAttachedTo->m_vecLastCollisionImpactVelocity, &matrix->GetUp()) < 0.7f
                && 0.3f * 0.3f > m_pAttachedTo->m_vecMoveSpeed.SquaredMagnitude()) {
                bDettachEntity = true;
            } else {
                bUpdateSpeed = true;
            }
        }
    }

    if (!bUpdateSpeed) {
        auto* attachedTo = m_pAttachedTo;
        DettachAutoAttachedEntity();
        if (!physicalFlags.bDisableCollisionForce) {
            float randomNumber = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
            const CMatrix& attachedToMatrix = attachedTo->GetMatrix();
            CVector randomRight = CVector{ randomNumber * attachedToMatrix.GetRight().x, randomNumber * attachedToMatrix.GetRight().y, randomNumber * attachedToMatrix.GetRight().z };
            const CMatrix& attachedToMatrix2 = attachedTo->GetMatrix();
            CVector randomForward = CVector{ randomNumber * attachedToMatrix2.GetForward().x, randomNumber * attachedToMatrix2.GetForward().y, randomNumber * attachedToMatrix2.GetForward().z };
            // Como el original: masa * 0.01 (0x858C58), no masa / 50
            const float fForceMult = m_fMass * 0.01f;
            CVector force = (randomRight + randomForward) * fForceMult;
            ApplyMoveForce(force);
            if (attachedToAuto->m_wMiscComponentAngle > attachedToAuto->m_wMiscComponentAnglePrev) {
                // Como el original: up * (masa * 0.02) (0x858B38)
                const float fUpMult = m_fMass * 0.02f;
                ApplyMoveForce(fUpMult * attachedTo->GetMatrix().GetUp());
            }
        }
        return;
    }

    if (physicalFlags.bInfiniteMass) {
        CVector vecMoveSpeed = GetPosition() - m_vecAttachedEntityRotation;
        float fSquaredMagnitude = vecMoveSpeed.SquaredMagnitude();
        m_vecAttachedEntityRotation = GetPosition();
        if (fSquaredMagnitude > 4.0f)
            vecMoveSpeed *= 2.0f / sqrt(fSquaredMagnitude);

        // Como el original: un paso NaN se usa tal cual
        float fTimeStep = 1.0f;
        if (!(1.0f > CTimer::GetTimeStep()))
            fTimeStep = CTimer::GetTimeStep();

        vecMoveSpeed *= 1.0f / fTimeStep;
        CVector vecMoveSpeedDifference = vecMoveSpeed - m_vecMoveSpeed;
        m_vecMoveSpeed = vecMoveSpeed;
        CVector vecForce = vecMoveSpeedDifference * -m_fMass;
        // Como el original: Multiply3x3 (0x59C790)
        CVector vecCenterOfMassMultiplied = PhysAttachMat3x3_59C790(*m_matrix, m_vecCentreOfMass);
        ApplyForce(vecForce, vecCenterOfMassMultiplied, true);
        if (m_pAttachedTo->GetIsTypeVehicle() || m_pAttachedTo->GetIsTypeObject()) {
            if (m_pAttachedTo->GetUsesCollision() && !m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                CVector vecDistance = (vecCenterOfMassMultiplied + GetPosition()) - m_pAttachedTo->GetPosition();
                m_pAttachedTo->ApplyForce(vecForce * -1.0f, vecDistance, true);
            }
            // Como el original: los dos headings de fpatan se restan sin redondear a float (x87)
            const CPlaceable* const attachedTo = m_pAttachedTo;
            const CPlaceable* const self = this;
            float fRotationInRadians;
            __asm {
                mov ecx, attachedTo
                mov eax, dword ptr [ecx+14h]
                test eax, eax
                je at_no_matrix
                fld dword ptr [eax+14h]
                fld dword ptr [eax+10h]
                fchs
                fxch st(1)
                fpatan
                jmp at_done
            at_no_matrix:
                fld dword ptr [ecx+10h]
            at_done:
                mov ecx, self
                mov eax, dword ptr [ecx+14h]
                test eax, eax
                je self_no_matrix
                fld dword ptr [eax+14h]
                fld dword ptr [eax+10h]
                fchs
                fxch st(1)
                fpatan
                jmp self_done
            self_no_matrix:
                fld dword ptr [ecx+10h]
            self_done:
                fsubp st(1), st
                fstp fRotationInRadians
            }
            // Como el original: pi y 2pi exactos del exe (0x858CB8 / 0x858CBC / 0x858CC0)
            const float fPi    = std::bit_cast<float>(0x40490FDBu);
            const float fTwoPi = std::bit_cast<float>(0x40C90FDBu);
            if (fRotationInRadians > fPi)
                fRotationInRadians -= fTwoPi;
            else if (fRotationInRadians < -fPi)
                fRotationInRadians += fTwoPi;
            fRotationInRadians = std::clamp(fRotationInRadians, -0.5f, 0.5f);
            // Como el original: * 1e-5 (0x8CD7DC) en lugar de / 100000
            m_vecTurnSpeed.z += fRotationInRadians * std::bit_cast<float>(0x3727C5ACu) * m_fMass;
        }
    }
    else {
        if (m_pAttachedTo->GetIsTypeVehicle() || m_pAttachedTo->GetIsTypeObject()) {
            m_vecMoveSpeed = m_pAttachedTo->GetSpeed(GetPosition() - m_pAttachedTo->GetPosition());
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    }
}

// 0x547B80
void CPhysical::ApplySpeed()
{
    // Como el original: constantes exactas del exe (0x858CB8 pi, 0x858CBC 2pi, 0x8CD7E8 108 grados, 0x863B64 0.06, 0x863B6C 0.03)
    const float fPi        = std::bit_cast<float>(0x40490FDBu);
    const float fTwoPi     = std::bit_cast<float>(0x40C90FDBu);
    const float fDoorLimit = std::bit_cast<float>(0x3FF1463Bu);

    auto* object = AsObject();
    float fOldTimeStep = CTimer::GetTimeStep();
    if (physicalFlags.bDisableZ) {
        if (physicalFlags.bApplyGravity) {
            if (fOldTimeStep * m_vecMoveSpeed.z + GetPosition().z < CWorld::SnookerTableMin.z) {
                GetPosition().z = CWorld::SnookerTableMin.z;
                ResetMoveSpeed();
                ResetTurnSpeed();
            }
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            CTimer::UpdateTimeStep(fOldTimeStep);
            return;
        }

        // Como el original: las comparaciones del exe tratan NaN como "no supera el límite"
        float fTimeStepX = 1000.0f;
        float fTimeStepY = 1000.0f;
        float fNewPositionX = fOldTimeStep * m_vecMoveSpeed.x + GetPosition().x;
        if (fNewPositionX > CWorld::SnookerTableMax.x && m_vecMoveSpeed.x > 0.0f) {
            fTimeStepX = (CWorld::SnookerTableMax.x - GetPosition().x) / m_vecMoveSpeed.x;
        } else if (fNewPositionX < CWorld::SnookerTableMin.x && m_vecMoveSpeed.x < 0.0f) {
            fTimeStepX = (CWorld::SnookerTableMin.x - GetPosition().x) / m_vecMoveSpeed.x;
        }

        float fNewPositionY = CTimer::GetTimeStep() * m_vecMoveSpeed.y + GetPosition().y;
        if (fNewPositionY > CWorld::SnookerTableMax.y && m_vecMoveSpeed.y > 0.0f) {
            fTimeStepY = (CWorld::SnookerTableMax.y - GetPosition().y) / m_vecMoveSpeed.y;
        } else if (fNewPositionY < CWorld::SnookerTableMin.y && m_vecMoveSpeed.y < 0.0f) {
            fTimeStepY = (CWorld::SnookerTableMin.y - GetPosition().y) / m_vecMoveSpeed.y;
        }

        bool bTableWidthIsLessThanHeight = true;
        if (CWorld::SnookerTableMax.x - CWorld::SnookerTableMin.x < CWorld::SnookerTableMax.y - CWorld::SnookerTableMin.y)
            bTableWidthIsLessThanHeight = false;
        bool bApplyFriction = false;
        float fNormalX = 0.0f;
        float fNormalY = 0.0f;
        float fAbsoluteMoveSpeed = 0.0f; // Como el original: el exe no lo inicializa, pero solo se lee si hubo rebote
        float fNewTimeStep = 0.0f;
        if (fTimeStepX < fTimeStepY && fTimeStepX < 1000.0f) {
            fNormalX = -1.0f;
            fAbsoluteMoveSpeed = fabs(m_vecMoveSpeed.x);
            if (!(m_vecMoveSpeed.x > 0.0f))
                fNormalX = 1.0f;
            CTimer::UpdateTimeStep(fTimeStepX);
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            const float fPosY = GetPosition().y;
            bool bReverse = false;
            if (!(CWorld::SnookerTableMax.y - 0.06f < fPosY) && !(0.06f + CWorld::SnookerTableMin.y > fPosY)) {
                if (bTableWidthIsLessThanHeight) {
                    bReverse = true;
                } else {
                    float fTableY = (CWorld::SnookerTableMin.y + CWorld::SnookerTableMax.y) * 0.5f;
                    bReverse = !(fTableY - 0.06f < fPosY) || !(fPosY < 0.06f + fTableY);
                }
            }
            if (bReverse) {
                m_vecMoveSpeed.x = m_vecMoveSpeed.x * -1.0f;
            } else {
                physicalFlags.bApplyGravity = true;
                float fTimeStepMoveSpeedX = fOldTimeStep * m_vecMoveSpeed.x;
                if (fTimeStepMoveSpeedX > 0.03f) {
                    m_vecMoveSpeed.x = 0.03f / fOldTimeStep;
                } else if (fTimeStepMoveSpeedX < -0.03f) {
                    m_vecMoveSpeed.x = -(0.03f / fOldTimeStep);
                }
            }
            fNewTimeStep = fOldTimeStep - fTimeStepX;
            bApplyFriction = true;
        }
        if (!bApplyFriction && fTimeStepY < 1000.0f) {
            fNormalY = -1.0f;
            fAbsoluteMoveSpeed = fabs(m_vecMoveSpeed.y);
            if (!(m_vecMoveSpeed.y > 0.0f))
                fNormalY = 1.0f;
            CTimer::UpdateTimeStep(fTimeStepY);
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            const float fPosX = GetPosition().x;
            float fTableX = (CWorld::SnookerTableMin.x + CWorld::SnookerTableMax.x) * 0.5f;
            if (CWorld::SnookerTableMax.x - 0.06f < fPosX
                || 0.06f + CWorld::SnookerTableMin.x > fPosX
                || bTableWidthIsLessThanHeight && fTableX - 0.06f < fPosX && fPosX < 0.06f + fTableX) {
                physicalFlags.bApplyGravity = true;
                float fTimeStepMoveSpeedY = fOldTimeStep * m_vecMoveSpeed.y;
                if (fTimeStepMoveSpeedY > 0.03f) {
                    m_vecMoveSpeed.y = 0.03f / fOldTimeStep;
                } else if (fTimeStepMoveSpeedY < -0.03f) {
                    m_vecMoveSpeed.y = -(0.03f / fOldTimeStep);
                }
            }
            else {
                m_vecMoveSpeed.y = m_vecMoveSpeed.y * -1.0f;
            }
            fNewTimeStep = fOldTimeStep - fTimeStepY;
            bApplyFriction = true;
        }

        if (bApplyFriction) {
            CTimer::UpdateTimeStep(fNewTimeStep);
            if (fAbsoluteMoveSpeed > 0.0f) {
                float fRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
                // Como el original: la z también se calcula como 0.0f * radio
                CVector thePosition = CVector(fNormalX * fRadius, fNormalY * fRadius, fRadius * 0.0f);
                CColPoint colPoint{};
                colPoint.m_vecPoint = GetPosition() - thePosition;
                colPoint.m_vecNormal = CVector(fNormalX, fNormalY, 0.0f);
                ApplyFriction(10.0f * fAbsoluteMoveSpeed, colPoint);
                if (GetIsTypeObject())
                {
                    AudioEngine.ReportMissionAudioEvent(AE_SCRIPT_POOL_HIT_CUSHION, object);
                    object->m_nLastWeaponDamage = 4 * (object->m_nLastWeaponDamage == 0xFF) + WEAPON_RUNOVERBYCAR;
                }
            }
        }
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        CTimer::UpdateTimeStep(fOldTimeStep);
        return;
    }

    // Como el original: m_fDoorStartAngle NaN se trata como <= -1000
    if (!physicalFlags.bDisableMoveForce || !GetIsTypeObject() || !(object->m_fDoorStartAngle > -1000.0f)) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        CTimer::UpdateTimeStep(fOldTimeStep);
        return;
    }

    float fDoorStartAngle = object->m_fDoorStartAngle;
    float fHeading = GetHeading();
    if (fDoorStartAngle + fPi < fHeading) {
        fHeading = fHeading - fTwoPi;
    } else if (fDoorStartAngle - fPi > fHeading) {
        fHeading = fHeading + fTwoPi;
    }

    float fNewTimeStep = -1000.0f;
    if (m_vecTurnSpeed.z > 0.0f) {
        float fTheDoorAngle = fDoorLimit + fDoorStartAngle;
        if (CTimer::GetTimeStep() * m_vecTurnSpeed.z + fHeading > fTheDoorAngle)
            fNewTimeStep = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
    } else if (m_vecTurnSpeed.z < 0.0f) {
        float fTheDoorAngle = fDoorStartAngle - fDoorLimit;
        if (CTimer::GetTimeStep() * m_vecTurnSpeed.z + fHeading < fTheDoorAngle)
            fNewTimeStep = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
    }
    if (-CTimer::GetTimeStep() <= fNewTimeStep) {
        CTimer::UpdateTimeStep(fNewTimeStep);
        ApplyTurnSpeed();
        m_vecTurnSpeed.z = -0.2f * m_vecTurnSpeed.z;
        CTimer::UpdateTimeStep(fOldTimeStep - fNewTimeStep);
        physicalFlags.bDoorHitEndStop = true;
    }
    ApplyMoveSpeed();
    ApplyTurnSpeed();
    CTimer::UpdateTimeStep(fOldTimeStep);
    if (object->objectFlags.bIsDoorMoving) {
        // Como el original: el exe llama a CPlaceable::GetHeading (0x441DB0) y usa el resultado de fpatan
        // sin redondear a float en ST0 para el ajuste de +-2pi y la resta del ángulo inicial.
        const CPlaceable* const self = this;
        float fNewHeading;
        __asm {
            mov ecx, self
            mov eax, dword ptr [ecx+14h]
            test eax, eax
            je no_matrix
            fld dword ptr [eax+10h]
            fchs
            fld dword ptr [eax+14h]
            fpatan
            jmp have_heading
        no_matrix:
            fld dword ptr [ecx+10h]
        have_heading:
            fld fPi
            fadd st, st(1)
            fcomp fDoorStartAngle
            fnstsw ax
            test ah, 5
            jp not_low
            fadd fTwoPi
            jmp wrapped
        not_low:
            fld fPi
            fsubr st, st(1)
            fcomp fDoorStartAngle
            fnstsw ax
            test ah, 41h
            jne wrapped
            fsub fTwoPi
        wrapped:
            fsub fDoorStartAngle
            fstp fNewHeading
        }

        fHeading -= fDoorStartAngle;
        if (fabs(fHeading) < 0.001f)
            fHeading = 0.0f;
        if (fabs(fNewHeading) < 0.001f)
            fNewHeading = 0.0f;
        if (fHeading * fNewHeading < 0.0f)
            m_vecTurnSpeed.z = 0.0f;
    }
}

// 0x548320
void CPhysical::UnsetIsInSafePosition()
{
    m_vecMoveSpeed *= -1.0f;
    m_vecTurnSpeed *= -1.0f;
    ApplySpeed();
    m_vecMoveSpeed *= -1.0f;
    m_vecTurnSpeed *= -1.0f;
    SetIsInSafePosition(false);
}

// 0x5483D0
void CPhysical::ApplyFriction()
{
    if (physicalFlags.bDisableZ)
    {
        const CVector& vecPosition = GetPosition();
        float fSphereRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
        CColPoint colPoint;
        colPoint.m_vecPoint.x = vecPosition.x - (0.0f * fSphereRadius);
        colPoint.m_vecPoint.y = vecPosition.y - (0.0f * fSphereRadius);
        colPoint.m_vecPoint.z = vecPosition.z - fSphereRadius;
        colPoint.m_vecNormal = CVector (0.0f, 0.0f, 1.0f);

        ApplyFriction(CTimer::GetTimeStep() * 0.001f, colPoint);
        // Como el original: pow(0.98, paso) con el _CIpow del EXE (0x822130) en ST0 completo por turnSpeed.z
        const float fBase = 0.98f;
        const float fStep = CTimer::GetTimeStep();
        float* const pTurnZ = &m_vecTurnSpeed.z;
        __asm {
            fld fBase
            fld fStep
            mov eax, 0x822130
            call eax
            mov edx, pTurnZ
            fmul dword ptr [edx]
            fstp dword ptr [edx]
        }
    }

    m_vecMoveSpeed += m_vecFrictionMoveSpeed;
    m_vecTurnSpeed += m_vecFrictionTurnSpeed;
    ResetFrictionMoveSpeed();
    ResetFrictionTurnSpeed();

    auto* vehicle = AsVehicle();
    if (GetIsTypeVehicle() && vehicle->IsBike()
        && !physicalFlags.bCarriedByRope && GetStatus() == STATUS_ABANDONED
        && fabs(GetUp().z) < 0.707f
        // Como el original: umbrales y base leídos de las variables 0x8CD7F4, 0x8CD7F0 y 0x8CD7EC
        && StaticRef<float>(0x8CD7F4) * StaticRef<float>(0x8CD7F4) > m_vecMoveSpeed.SquaredMagnitude()
        && StaticRef<float>(0x8CD7F0) * StaticRef<float>(0x8CD7F0) > m_vecTurnSpeed.SquaredMagnitude())
    {
        // Como el original: pow con el _CIpow del EXE (0x822130), redondeado a float para CVector::operator*=
        const float fBase = StaticRef<float>(0x8CD7EC);
        const float fStep = CTimer::GetTimeStep();
        float fPow;
        __asm {
            fld fBase
            fld fStep
            mov eax, 0x822130
            call eax
            fstp fPow
        }
        m_vecMoveSpeed *= fPow;
    }
}

// 0x548680
bool CPhysical::ApplyCollision(CEntity* theEntity, CColPoint& colPoint, float& thisDamageIntensity, float& entityDamageIntensity)
{
    auto* entity = theEntity->AsPhysical();
    auto* entityObject = entity->AsObject();
    auto* entityVehicle = entity->AsVehicle();

    auto* thisObject = AsObject();
    auto* thisPed = AsPed();
    auto* thisVehicle = AsVehicle();

    bool bThisPedIsStandingOnEntity = false;
    bool bEntityPedIsStandingOnThis = false;
    bool bEntityCollisionForceDisabled = false;

    float fThisMassFactor = 0.0f;
    float fEntityMassFactor = 1.0f;

    if (!entity->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce)
    {
        fThisMassFactor = 2.0f;
        if (!physicalFlags.bMakeMassTwiceAsBig)
        {
            fThisMassFactor = 1.0f;
        }
    }
    else
    {
        fThisMassFactor = 10.0f;
        if (entity->GetIsTypePed() && entity->AsPed()->m_standingOnEntity == this)
        {
            bEntityPedIsStandingOnThis = true;
        }
    }

    if (physicalFlags.bDisableTurnForce)
    {
        if (GetIsTypePed() && thisPed->IsPlayer()
            && entity->GetIsTypeVehicle()
            && (entity->GetStatus() == STATUS_ABANDONED || entity->GetStatus() == STATUS_WRECKED || GetHasHitWall())) // Como el original: prueba el bit 12 de 0x1C (m_bHasHitWall) de this, no m_bIsStuck
        {
            float fTheEntityMass = entity->m_fMass - 2000.0f;
            if (fTheEntityMass < 0.0f)
            {
                fTheEntityMass = 0.0f;
            }
            fEntityMassFactor = 1.0f / (fTheEntityMass * 0.0002f + 1.0f); // Como el original: multiplica por 0.0002f (0x85A7D0) en vez de dividir entre 5000
        }
        else if (!entity->physicalFlags.bDisableMoveForce)
        {
            fEntityMassFactor = 10.0f;
        }
        if (GetIsTypePed() && thisPed->m_standingOnEntity == entity)
        {
            bThisPedIsStandingOnEntity = true;
            fEntityMassFactor = 10.0f;
        }
    }
    else if (GetIsTypeVehicle() && thisVehicle->m_pVehicleBeingTowed)
    {
        fEntityMassFactor = (thisVehicle->m_pVehicleBeingTowed->m_fMass + m_fMass) / m_fMass;
    }
    else
    {
        fEntityMassFactor = 2.0f;
        if (!entity->physicalFlags.bMakeMassTwiceAsBig)
        {
            fEntityMassFactor = 1.0f;
        }
    }

    if (entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bCollidable
        || entity->m_pAttachedTo && !entity->physicalFlags.bInfiniteMass)
    {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }

    // Como el original: Multiply3x3 (0x59C790) con su orden de suma propio
    const auto Multiply3x3 = [](const CMatrix& m, const CVector& v) {
        CVector out;
        out.x = m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x;
        out.y = m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y;
        out.z = m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y;
        return out;
    };
    CVector vecThisCentreOfMassMultiplied = Multiply3x3(GetMatrix(), m_vecCentreOfMass);
    CVector vecEntityCentreOfMassMultiplied = Multiply3x3(entity->GetMatrix(), entity->m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass)
    {
        vecThisCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    if (entity->physicalFlags.bInfiniteMass)
    {
        vecEntityCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    if (GetIsTypeVehicle() && entity->GetIsTypeObject()
        && entityObject->objectFlags.bIsLampPost
        || entity->GetIsTypeVehicle() && GetIsTypeObject() &&
        thisObject->objectFlags.bIsLampPost)
    {
        colPoint.m_vecNormal.z = 0.0f;
    }

    if (entity->GetIsStatic() && !bEntityCollisionForceDisabled)
    {
        if (physicalFlags.bDisableTurnForce)
        {
            float fThisSpeedDotProduct = DotProduct(m_vecMoveSpeed, colPoint.m_vecNormal);
            if (fThisSpeedDotProduct < 0.0f)
            {
                if (!entity->GetIsTypeObject())
                {
                    if (entity->physicalFlags.bDisableCollisionForce)
                    {
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    entity->SetIsStatic(false);
                }
                else
                {
                    thisDamageIntensity = -(fThisSpeedDotProduct * m_fMass);
                    entityDamageIntensity = thisDamageIntensity;

                    if (entity->physicalFlags.bDisableCollisionForce)
                    {
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    CObjectData* entityObjectInfo = entityObject->m_pObjectInfo;
                    if (!(entityObjectInfo->m_fUprootLimit < 9999.0f) || !(thisDamageIntensity > entityObjectInfo->m_fUprootLimit)) // Como el original: comparaciones x87 sensibles a NaN
                    {
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    if (CGlass::IsObjectGlass(entity))
                    {
                        CGlass::WindowRespondsToCollision(entity, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    }
                    else
                    {
                        if (!entity->physicalFlags.bDisableCollisionForce)
                        {
                            entity->SetIsStatic(false);
                            CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                            CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(rand() % 30 + 30)); // Como el original: rand (0x821B1E) % 30 + 30
                        }
                    }
                }
            }
        }
        else
        {
            CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
            CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

            if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

            float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
            if (fThisSpeedDotProduct < 0.0f)
            {
                if (!entity->GetIsTypeObject())
                {
                    if (entity->physicalFlags.bDisableCollisionForce)
                    {
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    entity->SetIsStatic(false);
                    CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                    CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(rand() % 30 + 30)); // Como el original: rand (0x821B1E) % 30 + 30
                }
                else
                {
                    CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
                    CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
                    float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
                    float fThisCollisionMass = 1.0f / (squaredMagnitude / m_fTurnMass + 1.0f / m_fMass);
                    if (!GetHasHitWall())
                    {
                        thisDamageIntensity = -((m_fElasticity + 1.0f) * fThisCollisionMass * fThisSpeedDotProduct);
                    }
                    else
                    {
                        thisDamageIntensity = fThisCollisionMass * fThisSpeedDotProduct * -1.0f;
                    }

                    entityDamageIntensity = thisDamageIntensity;

                    CObjectData* entityObjectInfo = entityObject->m_pObjectInfo;

                    float fObjectDamageMultiplier = 1.0f;
                    if (GetIsTypeVehicle() && thisVehicle->IsSubBike())
                    {
                        fObjectDamageMultiplier = 3.0f;
                    }

                    if (entityObject->m_nColDamageEffect)
                    {
                        float fObjectDamage = fObjectDamageMultiplier * thisDamageIntensity;
                        if (fObjectDamage > 20.0f)
                        {
                            entityObject->ObjectDamage(fObjectDamage, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                            if (!entity->GetUsesCollision())
                            {
                                if (!physicalFlags.bDisableCollisionForce)
                                {
                                    float fColDamageMultiplier = entityObjectInfo->m_fColDamageMultiplier;
                                    float fCollisionDamage = fColDamageMultiplier + fColDamageMultiplier;
                                    // Como el original: operator/ (0x4119D0) multiplica por el recíproco 1/k
                                    const float fInvCollisionDamage = 1.0f / fCollisionDamage;
                                    CVector vecMoveForce = colPoint.m_vecNormal * thisDamageIntensity;
                                    vecMoveForce = CVector{ vecMoveForce.x * fInvCollisionDamage, vecMoveForce.y * fInvCollisionDamage, vecMoveForce.z * fInvCollisionDamage };
                                    ApplyForce(vecMoveForce, vecDistanceToPointFromThis, true);
                                }

                                float fDamageIntensityMultiplier = entityObjectInfo->m_fColDamageMultiplier / fThisCollisionMass;
                                float fCollisionImpact1 = fDamageIntensityMultiplier * thisDamageIntensity + fDamageIntensityMultiplier * thisDamageIntensity;

                                AudioEngine.ReportCollision(
                                    this,
                                    entity,
                                    colPoint.m_nSurfaceTypeA,
                                    colPoint.m_nSurfaceTypeB,
                                    colPoint.m_vecPoint,
                                    &colPoint.m_vecNormal,
                                    fCollisionImpact1,
                                    1.0f,
                                    false,
                                    false
                                );
                                return false;
                            }
                        }
                    }
                    // Como el original: comparaciones x87 sensibles a NaN
                    if (entity->physicalFlags.bDisableCollisionForce || !(entityObjectInfo->m_fUprootLimit < 9999.0f)
                        || !(thisDamageIntensity > entityObjectInfo->m_fUprootLimit) && (!GetIsStuck() || !GetHasHitWall()))
                    {
                        if (CGlass::IsObjectGlass(entity))
                        {
                            CGlass::WindowRespondsToSoftCollision(entityObject, thisDamageIntensity);
                        }
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    if (CGlass::IsObjectGlass(entity))
                    {
                        CGlass::WindowRespondsToCollision(entity, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    }
                    else
                    {
                        entity->SetIsStatic(false);
                    }

                    int32 entityModelIndex = static_cast<int16>(entity->m_nModelIndex); // Como el original: movsx
                    if (entityModelIndex != ModelIndices::MI_FIRE_HYDRANT || entityObject->objectFlags.bIsExploded)
                    {
                        if (entityModelIndex != ModelIndices::MI_PARKINGMETER && entityModelIndex != ModelIndices::MI_PARKINGMETER2 || entityObject->objectFlags.bIsExploded) {
                            if (!entity->GetIsTypeObject() || entityObjectInfo->m_bCausesExplosion) {
                                // nothing
                            } else {
                                entityObject->objectFlags.bIsExploded = true;
                            }
                        } else {
                            CPickups::CreateSomeMoney(entity->GetPosition(), CGeneral::GetRandomNumber() % 100);
                            entityObject->objectFlags.bIsExploded = true;
                        }
                    }
                    else
                    {
                        g_fx.TriggerWaterHydrant(entity->GetPosition());
                        entityObject->objectFlags.bIsExploded = true;
                    }

                    if (!physicalFlags.bDisableCollisionForce && entityObjectInfo->m_fUprootLimit > 200.0f)
                    {
                        CVector vecMoveForce = (colPoint.m_vecNormal * 0.2f) * thisDamageIntensity;
                        ApplyForce(vecMoveForce, vecDistanceToPointFromThis, true);
                    }
                }
            }
        }

        if (entity->GetIsStatic())
        {
            return false;
        }
        if (!entity->physicalFlags.bDisableCollisionForce)
        {
            entity->AddToMovingList();
        }
    }

    if (physicalFlags.bDisableTurnForce)
    {
        if (entity->physicalFlags.bDisableTurnForce)
        {
            bool bApplyEntityCollisionForce = true;
            float fThisMass = m_fMass;
            float fEntityMass = entity->m_fMass;
            float fThisSpeedDotProduct = DotProduct(m_vecMoveSpeed, colPoint.m_vecNormal);
            float fEntitySpeedDotProduct = DotProduct(entity->m_vecMoveSpeed, colPoint.m_vecNormal);
            float fMoveSpeed = 0.0f;
            if (physicalFlags.bDisableCollisionForce || physicalFlags.bDontApplySpeed)
            {
                fMoveSpeed = fThisSpeedDotProduct;
            }
            else
            {
                if (entity->physicalFlags.bDisableCollisionForce || entity->physicalFlags.bDontApplySpeed)
                {
                    fMoveSpeed = fEntitySpeedDotProduct;
                    bApplyEntityCollisionForce = false;
                }
                else
                {
                    if (!thisPed->bPushOtherPeds)
                    {
                        if (fEntitySpeedDotProduct < 0.0f) { // Como el original: NaN -> 0
                            fMoveSpeed = fEntitySpeedDotProduct;
                        } else {
                            fMoveSpeed = 0.0f;
                        }

                        bApplyEntityCollisionForce = false;
                    } else {
                        fMoveSpeed = (fThisMass * fThisSpeedDotProduct * 4.0f + fEntityMass * fEntitySpeedDotProduct) / (fThisMass * 4.0f + fEntityMass);
                    }
                }
            }

            float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
            if (!(fThisMoveSpeedDifference < 0.0f)) { // Como el original: NaN -> return false
                return false;
            }

            float fThisMoveSpeedElasticity = 0.0f;
            float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
            if (GetHasHitWall()) {
                fThisMoveSpeedElasticity = fMoveSpeed;
            } else {
                fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
            }

            thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;

            CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;
            if (!physicalFlags.bDisableCollisionForce && !physicalFlags.bDontApplySpeed)
            {
                ApplyMoveForce(vecThisMoveForce);

                float fCollisionImpact1 = thisDamageIntensity / fThisMass;
                AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            }

            if (bApplyEntityCollisionForce)
            {
                float fEntityMoveSpeedElasticity = 0.0f;
                if (entity->GetHasHitWall()) {
                    fEntityMoveSpeedElasticity = fMoveSpeed;
                } else {
                    fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
                }

                entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);


                CVector vecEntityMoveForce = colPoint.m_vecNormal * entityDamageIntensity * -1.0f;
                if (!entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bDontApplySpeed)
                {
                    if (entity->GetIsInSafePosition())
                    {
                        entity->UnsetIsInSafePosition();
                    }

                    entity->ApplyMoveForce(vecEntityMoveForce);

                    float fCollisionImpact1 = entityDamageIntensity / fEntityMass;
                    AudioEngine.ReportCollision(
                        entity,
                        this,
                        colPoint.m_nSurfaceTypeB,
                        colPoint.m_nSurfaceTypeA,
                        colPoint.m_vecPoint,
                        &colPoint.m_vecNormal,
                        fCollisionImpact1,
                        1.0f,
                        false,
                        false
                    );
                }
            }
            return true;
        }
    }

    if (physicalFlags.bDisableTurnForce)
    {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);

        if (!entity->physicalFlags.bAddMovingCollisionSpeed || !entity->GetIsTypeVehicle() || colPoint.m_nSurfaceTypeB != SURFACE_CAR_MOVINGCOMPONENT) { // Como el original: comprueba el tipo de entity (0x54934F)
            // nothing
        } else {
            vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);
        }

        float fThisSpeedDotProduct = DotProduct(m_vecMoveSpeed, colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(vecEntitySpeed, colPoint.m_vecNormal);

        float fThisMass = fThisMassFactor * m_fMass;

        CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct = CrossProduct(vecEntityDifference, colPoint.m_vecNormal);
        float squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = 0.0f;
        if (entity->physicalFlags.bDisableMoveForce) {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass);
        } else {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass) + 1.0f / (fEntityMassFactor * entity->m_fMass);
        }

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;

        float fMoveSpeed = 0.0f;
        if (bEntityCollisionForceDisabled) {
            fMoveSpeed = fEntitySpeedDotProduct;
        } else {
            fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisMass);
        }

        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (fThisMoveSpeedDifference < 0.0f)
        {
            float fThisMoveSpeedElasticity = 0.0f;
            float fEntityMoveSpeedElasticity = 0.0f;

            float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
            if (GetHasHitWall()) {
                fThisMoveSpeedElasticity = fMoveSpeed;
            } else {
                fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
            }

            if (entity->GetHasHitWall()) {
                fEntityMoveSpeedElasticity = fMoveSpeed;
            } else {
                fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
            }

            thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
            entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

            CVector vecThisMoveForce = (thisDamageIntensity / fThisMassFactor) * colPoint.m_vecNormal;
            CVector vecEntityMoveForce = (colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f);

            if (!physicalFlags.bDisableCollisionForce)
            {
                if (vecThisMoveForce.z < 0.0f)
                {
                    vecThisMoveForce.z = 0.0f;
                }
                if (bThisPedIsStandingOnEntity)
                {
                    vecThisMoveForce.x += vecThisMoveForce.x;
                    vecThisMoveForce.y += vecThisMoveForce.y;
                }

                ApplyMoveForce(vecThisMoveForce);
            }
            if (!entity->physicalFlags.bDisableCollisionForce && !bThisPedIsStandingOnEntity)
            {
                if (entity->GetIsInSafePosition())
                {
                    entity->UnsetIsInSafePosition();
                }
                entity->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
            }

            float fCollisionImpact1 = thisDamageIntensity / fThisMass;
            AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

            fCollisionImpact1 = entityDamageIntensity / fEntityCollisionMass;
            AudioEngine.ReportCollision(entity, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            return true;
        }
        return false;
    }

    if (entity->physicalFlags.bDisableTurnForce)
    {
        CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
        CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

        if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) // Como el original: el bit debe estar activo (0x549751)
            vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

        float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(entity->m_vecMoveSpeed, colPoint.m_vecNormal);

        CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
        float fThisCollisionMass = 0.0f;
        if (physicalFlags.bDisableMoveForce)
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass);
        }
        else
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass) + 1.0f / (fThisMassFactor * m_fMass);
        }

        fThisCollisionMass = 1.0f / fThisCollisionMass;

        float fEntityMass = fEntityMassFactor * entity->m_fMass;
        float fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (!(fThisMoveSpeedDifference < 0.0f)) // Como el original: NaN -> return false
        {
            return false;
        }

        float fThisMoveSpeedElasticity = 0.0f;
        float fEntityMoveSpeedElasticity = 0.0f;

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
        if (GetHasHitWall())
        {
            fThisMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
        }

        if (entity->GetHasHitWall())
        {
            fEntityMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
        }

        thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
        entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;

        if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis)
        {
            if (vecThisMoveForce.z < 0.0f)
            {
                vecThisMoveForce.z = 0.0f;
            }
            ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
        }

        if (!entity->physicalFlags.bDisableCollisionForce)
        {
            if (vecEntityMoveForce.z < 0.0f)
            {
                vecEntityMoveForce.z = 0.0f;
                if (static_cast<double>(std::fabs(fThisSpeedDotProduct)) < 0.01) // Como el original: compara con el double 0.01 (0x85A308)
                {
                    vecEntityMoveForce.x *= 0.5f;
                    vecEntityMoveForce.y *= 0.5f;
                    vecEntityMoveForce.z  = 0.0f;
                }
            }
            if (bEntityPedIsStandingOnThis)
            {
                vecEntityMoveForce.x += vecEntityMoveForce.x;
                vecEntityMoveForce.y += vecEntityMoveForce.y;
            }
            if (entity->GetIsInSafePosition())
            {
                entity->UnsetIsInSafePosition();
            }

            entity->ApplyMoveForce(vecEntityMoveForce);
        }

        float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass;
        AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

        fCollisionImpact1 = entityDamageIntensity / fEntityMass;
        AudioEngine.ReportCollision(entity, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
    }
    else
    {
        CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
        CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

        if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
            vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);

        if (entity->physicalFlags.bAddMovingCollisionSpeed && entity->GetIsTypeVehicle() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
            vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);

        float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = DotProduct(vecEntitySpeed, colPoint.m_vecNormal);

        CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();

        float fThisCollisionMass = 0.0f;
        if (physicalFlags.bDisableMoveForce)
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass);
        }
        else
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass) + 1.0f / (fThisMassFactor * m_fMass);
        }

        fThisCollisionMass = 1.0f / fThisCollisionMass;

        CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct = CrossProduct(vecEntityDifference, colPoint.m_vecNormal);
        squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();

        float fEntityCollisionMass = 0.0f;
        if (entity->physicalFlags.bDisableMoveForce)
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass);
        }
        else
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass) + 1.0f / (fEntityMassFactor * entity->m_fMass);
        }

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;

        float fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (!(fThisMoveSpeedDifference < 0.0f)) // Como el original: NaN -> return false
        {
            return false;
        }

        float fThisMoveSpeedElasticity = 0.0f;
        float fEntityMoveSpeedElasticity = 0.0f;
        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
        if (GetHasHitWall())
        {
            fThisMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
        }

        if (entity->GetHasHitWall())
        {
            fEntityMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
        }

        thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
        entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;

        if (GetIsTypeVehicle() && !GetHasHitWall() && !physicalFlags.bDisableCollisionForce)
        {
            if (colPoint.m_vecNormal.z < 0.7f)
            {
                vecThisMoveForce.z *= 0.3f;
            }

            if (!GetStatus())
            {
                vecDistanceToPointFromThis *= 0.8f;
            }

            if (CWorld::bNoMoreCollisionTorque)
            {
                CVector vecFrictionForce = vecThisMoveForce * -0.3f;
                ApplyFrictionForce(vecFrictionForce, vecDistanceToPointFromThis);
            }
        }

        if (entity->GetIsTypeVehicle() && !entity->GetHasHitWall() && !entity->physicalFlags.bDisableCollisionForce)
        {
            if ((colPoint.m_vecNormal.z * -1.0f) < 0.7f)
            {
                vecEntityMoveForce.z *= 0.3f;
            }

            if (!entity->GetStatus())
            {
                vecDistanceToPoint *= 0.8f;
            }

            if (CWorld::bNoMoreCollisionTorque)
            {
                CVector vecFrictionForce = vecEntityMoveForce * -0.3f;
                entity->ApplyFrictionForce(vecFrictionForce, vecDistanceToPoint);
            }
        }

        if (CCheat::IsActive(CHEAT_CARS_FLOAT_AWAY_WHEN_HIT))
        {
            if (FindPlayerVehicle() == thisVehicle
                && entity->GetIsTypeVehicle() && entityVehicle->GetCreatedBy() != MISSION_VEHICLE)
            {
                entity->physicalFlags.bApplyGravity = false;
            }
            if (FindPlayerVehicle() == entityVehicle
                && GetIsTypeVehicle()
                && thisVehicle->GetCreatedBy() != MISSION_VEHICLE)
            {
                physicalFlags.bApplyGravity = false;
            }
        }

        if (!physicalFlags.bDisableCollisionForce)
        {
            ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
        }

        if (!entity->physicalFlags.bDisableCollisionForce)
        {
            if (entity->GetIsInSafePosition())
            {
                entity->UnsetIsInSafePosition();
            }
            entity->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
        }

        float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass;
        AudioEngine.ReportCollision(this, entity, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

        fCollisionImpact1 = entityDamageIntensity / fEntityCollisionMass;
        AudioEngine.ReportCollision(entity, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
    }
    return true;
}

// 0x54A2C0
bool CPhysical::ApplySoftCollision(CPhysical* physical, CColPoint& colPoint, float& thisDamageIntensity, float& entityDamageIntensity)
{
    auto* entityObject = physical->AsObject();
    auto* entityPed = physical->AsPed();
    auto* entityVehicle = physical->AsVehicle();

    auto* thisObject = AsObject();
    auto* thisPed = AsPed();
    auto* thisVehicle = AsVehicle();

    bool bEntityPedIsStandingOnThis = false;
    bool bThisPedIsStandingOnEntity = false;
    bool bEntityCollisionForceDisabled = false;

    float fThisMassFactor = 0.0f;
    float fEntityMassFactor = 0.0f;
    if (!physical->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce || physicalFlags.bInfiniteMass)
    {
        fThisMassFactor = 2.0f;
        if (!physicalFlags.bMakeMassTwiceAsBig)
        {
            fThisMassFactor = 1.0f;
        }
    }
    else
    {
        fThisMassFactor = 10.0f;
        if (entityPed->m_standingOnEntity == this) // BUG: Game should be checking if entity is ped or not.
        {
            bEntityPedIsStandingOnThis = true;
        }
    }
    if (!physicalFlags.bDisableTurnForce || physical->physicalFlags.bDisableMoveForce || physical->physicalFlags.bInfiniteMass)
    {
        fEntityMassFactor = 2.0f;
        if (!physical->physicalFlags.bMakeMassTwiceAsBig)
        {
            fEntityMassFactor = 1.0f;
        }
    }
    else
    {
        if (GetIsTypePed() && thisPed->IsPlayer()
            && physical->GetIsTypeVehicle()
            && (physical->GetStatus() == STATUS_ABANDONED || physical->GetStatus() == STATUS_WRECKED || GetHasHitWall())) // Como el original: test ah,10h sobre [this+1Ch] = m_bHasHitWall (0x54A360)
        {
            float fTheEntityMass = physical->m_fMass - 2000.0f;
            if (fTheEntityMass < 0.0f)
            {
                fTheEntityMass = 0.0f;
            }
            fEntityMassFactor = 1.0f / (fTheEntityMass * 0.0002f + 1.0f);
        }
        else
        {
            fEntityMassFactor = 10.0f;
        }
        if (GetIsTypePed() && thisPed->m_standingOnEntity == physical)
        {
            bThisPedIsStandingOnEntity = true;
        }
    }

    if (physical->physicalFlags.bDisableCollisionForce && !physical->physicalFlags.bCollidable)
    {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }

    // Como el original: Multiply3x3 (0x59C790) con su orden de sumas:
    // x = (up.x*v.z + fwd.x*v.y) + right.x*v.x; y = (up.y*v.z + right.y*v.x) + fwd.y*v.y; z = (up.z*v.z + right.z*v.x) + fwd.z*v.y
    const auto Multiply3x3Exe = [](const CMatrix& m, const CVector& v) {
        CVector out;
        out.x = m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x;
        out.y = m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y;
        out.z = m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y;
        return out;
    };
    CVector vecThisCentreOfMassMultiplied = Multiply3x3Exe(GetMatrix(), m_vecCentreOfMass);
    CVector vecEntityCentreOfMassMultiplied = Multiply3x3Exe(physical->GetMatrix(), physical->m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass)
    {
        vecThisCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }
    if (physical->physicalFlags.bInfiniteMass)
    {
        vecEntityCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    bool bApplyCollisionWithElasticity = false;
    if (!physical->GetIsStatic() || bEntityCollisionForceDisabled)
    {
        bApplyCollisionWithElasticity = true;
    }
    else
    {
        if (physicalFlags.bDisableTurnForce)
        {
            return ApplySoftCollision(physical, colPoint, thisDamageIntensity);
        }
    }

    // Como el original: la distancia, GetSpeed (0x542CE0) y el producto escalar solo se calculan en la rama
    // de entidad estática (0x54A4ED); el resto va directo a 0x54A88F.
    if (!bApplyCollisionWithElasticity)
    {
    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

    float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
    if (fThisSpeedDotProduct < 0.0f)
    {
        if (!physical->GetIsTypeObject())
        {
            if (!physical->physicalFlags.bDisableCollisionForce)
            {
                physical->SetIsStatic(false);
                CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                // Como el original: rand() % 30 + 30 con idiv (0x54A5C2), no la interpolación en float
                CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumber() % 30 + 30));
            }
        }
        else
        {
            CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
            CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
            float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();

            float fThisCollisionMass = 1.0f / (squaredMagnitude / m_fTurnMass + 1.0f / m_fMass);
            if (!GetHasHitWall())
            {
                thisDamageIntensity = -((m_fElasticity + 1.0f) * fThisCollisionMass * fThisSpeedDotProduct);
            }
            else
            {
                thisDamageIntensity = fThisCollisionMass * fThisSpeedDotProduct * -1.0f;
            }

            entityDamageIntensity = thisDamageIntensity;

            if (entityObject->m_nColDamageEffect != COL_DAMAGE_EFFECT_NONE && thisDamageIntensity > 20.0f)
            {
                entityObject->ObjectDamage(thisDamageIntensity, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                if (!physical->GetUsesCollision())
                {
                    if (!physicalFlags.bDisableCollisionForce)
                    {
                        CVector vecMoveSpeed = (colPoint.m_vecNormal * 0.2f) * thisDamageIntensity;
                        ApplyForce(vecMoveSpeed, vecDistanceToPointFromThis, true);
                    }

                    float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass * 0.2f;
                    AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                    return false;
                }
            }

            float fObjectUprootLimit = entityObject->m_pObjectInfo->m_fUprootLimit;
            if ((thisDamageIntensity > fObjectUprootLimit || GetIsStuck()) && !physical->physicalFlags.bDisableCollisionForce)
            {
                if (CGlass::IsObjectGlass(physical))
                {
                    CGlass::WindowRespondsToCollision(physical, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                }
                else if (!physical->physicalFlags.bDisableCollisionForce)
                {
                    physical->SetIsStatic(false);
                }

                // Como el original: movsx del índice de modelo y comparación con la variable MI_FIRE_HYDRANT (0x8CD594, movzx)
                int32 entityModelIndex = static_cast<int16>(physical->m_nModelIndex);
                if (entityModelIndex != ModelIndices::MI_FIRE_HYDRANT || entityObject->objectFlags.bIsExploded)
                {
                    if (entityModelIndex != ModelIndices::MI_PARKINGMETER && entityModelIndex != ModelIndices::MI_PARKINGMETER2 || entityObject->objectFlags.bIsExploded)
                    {
                        CBaseModelInfo* pBaseModelInfo = CModelInfo::GetModelInfo(entityModelIndex);
                        if (!physical->GetIsTypeObject() || pBaseModelInfo->AsAtomicModelInfoPtr())
                        {
                            // nothing
                        }
                        else
                        {
                            entityObject->objectFlags.bIsExploded = true;
                        }
                    }
                    else
                    {
                        CPickups::CreateSomeMoney(physical->GetPosition(), CGeneral::GetRandomNumber() % 100);
                        entityObject->objectFlags.bIsExploded = true;
                    }
                }
                else
                {
                    g_fx.TriggerWaterHydrant(physical->GetPosition());
                    entityObject->objectFlags.bIsExploded = true;
                }
                // Como el original: continúa en 0x54A86C (comprobación común tras la rama estática)
            }
            else
            {
                if (CGlass::IsObjectGlass(physical))
                {
                    CGlass::WindowRespondsToSoftCollision(entityObject, thisDamageIntensity);
                }

                CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;
                if (GetIsTypeVehicle() && colPoint.m_vecNormal.z < 0.7f)
                {
                    vecThisMoveForce.z *= 0.3f;
                }

                if (!physicalFlags.bDisableCollisionForce)
                {
                    bool bUpdateTorque = !GetIsTypeVehicle() || !CWorld::bNoMoreCollisionTorque;
                    ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, bUpdateTorque);
                }

                float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass;
                AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                return true; // Como el original: mov al,1 (0x54AAA8)
            }
        }
    }

    // Como el original (0x54A86C): también con producto >= 0 (o NaN) y tras la rama no-objeto; si la entidad sigue
    // estática devuelve false, si no la añade a la lista de movimiento y pasa a la colisión elástica (0x54A88F).
    if (physical->GetIsStatic())
    {
        return false;
    }

    if (!physical->physicalFlags.bDisableCollisionForce)
    {
        physical->AddToMovingList();
    }

    bApplyCollisionWithElasticity = true;
    }

    if (bApplyCollisionWithElasticity)
    {
        if (physicalFlags.bDisableTurnForce && physical->physicalFlags.bDisableTurnForce)
        {
            float fThisSpeedDotProduct = DotProduct(m_vecMoveSpeed, colPoint.m_vecNormal);
            float fEntitySpeedDotProduct = DotProduct(physical->m_vecMoveSpeed, colPoint.m_vecNormal);

            float fMoveSpeed = fEntitySpeedDotProduct;
            float fThisMass = m_fMass;
            float fEntityMass = physical->m_fMass;
            bool bPedFollowPath = false;
            if (thisPed->m_nPedState == PEDSTATE_FOLLOW_PATH)
            {
                bPedFollowPath = true;
                // Como el original: eM*eDot + (tM*tDot duplicado con fadd st0,st0) (0x54A901)
                const float fThisMassSpeed = fThisMass * fThisSpeedDotProduct;
                fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + (fThisMassSpeed + fThisMassSpeed))
                    / (fThisMass + fThisMass + fEntityMass);
            }
            else if (!(fEntitySpeedDotProduct < 0.0f)) // Como el original: test ah,5 / jp, NaN -> 0
            {
                fMoveSpeed = 0.0f;
            }

            float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
            if (!(fThisMoveSpeedDifference < 0.0f)) // Como el original: NaN también devuelve false (0x54AAEF)
            {
                return false;
            }

            float fThisMoveSpeedElasticity = 0.0f;
            float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
            if (GetHasHitWall())
            {
                fThisMoveSpeedElasticity = fMoveSpeed;
            }
            else
            {
                fThisMoveSpeedElasticity = fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;
            }

            thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
            CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;

            if (!physicalFlags.bDisableCollisionForce)
            {
                ApplyMoveForce(vecThisMoveForce);
            }

            float fCollisionImpact1 = thisDamageIntensity / fThisMass;
            AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            if (bPedFollowPath)
            {
                float fEntityMoveSpeedDifference = fEntitySpeedDotProduct - fMoveSpeed;
                if (fEntityMoveSpeedDifference < 0.0f)
                {
                    float fEntityMoveSpeedElasticity = 0.0f;
                    if (physical->GetHasHitWall())
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed - fEntityMoveSpeedDifference * fTheElasticity;
                    }

                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);
                    CVector vecEntityMoveForce = colPoint.m_vecNormal * entityDamageIntensity * -1.0f;
                    if (!physical->physicalFlags.bDisableCollisionForce)
                    {
                        physical->ApplyMoveForce(vecEntityMoveForce);
                    }

                    float fCollisionImpact1 = entityDamageIntensity / fEntityMass;
                    AudioEngine.ReportCollision(physical, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                }
            }
            return true;
        }

        if (physicalFlags.bDisableTurnForce)
        {
            CVector vecDistanceToPoint = colPoint.m_vecPoint - physical->GetPosition();
            CVector vecEntitySpeed = physical->GetSpeed(vecDistanceToPoint);

            if (!physical->physicalFlags.bAddMovingCollisionSpeed || !physical->GetIsTypeVehicle()
                || colPoint.m_nSurfaceTypeB != SURFACE_CAR_MOVINGCOMPONENT)
            {
                // nothing
            }
            else
            {
                vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);
            }

            float fThisMass = fThisMassFactor * m_fMass;


            float fThisSpeedDotProduct = DotProduct(m_vecMoveSpeed, colPoint.m_vecNormal);
            float fEntitySpeedDotProduct = DotProduct(vecEntitySpeed, colPoint.m_vecNormal);

            CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
            CVector vecEntityCrossProduct = CrossProduct(vecEntityDifference, colPoint.m_vecNormal);
            float squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();

            float fMoveSpeed = 0.0f;
            float fEntityCollisionMass = 1.0f / (squaredMagnitude / (fEntityMassFactor * physical->m_fTurnMass) + 1.0f / (fEntityMassFactor * physical->m_fMass));
            if (bEntityCollisionForceDisabled)
            {
                fMoveSpeed = fEntitySpeedDotProduct;
            }
            else
            {
                fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisMass);
            }

            float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
            if (fThisMoveSpeedDifference < 0.0f)
            {
                float fThisMoveSpeedElasticity = 0.0f;
                float fEntityMoveSpeedElasticity = 0.0f;
                float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                if (GetHasHitWall())
                {
                    fThisMoveSpeedElasticity = fMoveSpeed;
                }
                else
                {
                    fThisMoveSpeedElasticity = fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;
                }

                if (physical->GetHasHitWall())
                {
                    fEntityMoveSpeedElasticity = fMoveSpeed;
                }
                else
                {
                    fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
                }

                thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
                entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

                CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                CVector vecEntityMoveForce = (colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f);

                if (!physicalFlags.bDisableCollisionForce)
                {
                    if (vecThisMoveForce.z < 0.0f)
                    {
                        vecThisMoveForce.z = 0.0f;
                    }

                    if (bThisPedIsStandingOnEntity)
                    {
                        vecThisMoveForce.x = vecThisMoveForce.x + vecThisMoveForce.x;
                        vecThisMoveForce.y = vecThisMoveForce.y + vecThisMoveForce.y;
                    }

                    ApplyMoveForce(vecThisMoveForce);
                }

                if (physical->physicalFlags.bDisableCollisionForce || bThisPedIsStandingOnEntity)
                {
                    // nothing
                }
                else
                {
                    physical->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
                }
                float fCollisionImpact1 = thisDamageIntensity / fThisMass;
                AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

                fCollisionImpact1 = entityDamageIntensity / fEntityCollisionMass;
                AudioEngine.ReportCollision(physical, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                return true;
            }
        }
        else
        {
            if (physical->physicalFlags.bDisableTurnForce)
            {
                CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
                CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

                if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                    vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

                float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
                float fEntitySpeedDotProduct = DotProduct(physical->m_vecMoveSpeed, colPoint.m_vecNormal);

                CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
                CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
                float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
                float fThisCollisionMass = 1.0f / (squaredMagnitude / (fThisMassFactor * m_fTurnMass) + 1.0f / (fThisMassFactor * m_fMass));

                float fEntityMass = fEntityMassFactor * physical->m_fMass;
                float fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityMass + fThisCollisionMass);
                float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
                if (fThisMoveSpeedDifference < 0.0f)
                {
                    float fThisMoveSpeedElasticity = 0.0f;
                    float fEntityMoveSpeedElasticity = 0.0f;

                    float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                    if (GetHasHitWall())
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;
                    }
                    if (physical->GetHasHitWall())
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
                    }

                    thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);

                    CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                    CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;
                    if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis)
                    {
                        if (vecThisMoveForce.z < 0.0f)
                        {
                            vecThisMoveForce.z = 0.0f;
                        }
                        ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
                    }

                    if (!physical->physicalFlags.bDisableCollisionForce)
                    {
                        if (vecEntityMoveForce.z < 0.0f)
                        {
                            vecEntityMoveForce.z = 0.0f;
                            if (static_cast<double>(std::fabs(fThisSpeedDotProduct)) < 0.01) // Como el original: fcomp qword 0x85A308 (double 0.01)
                            {
                                vecEntityMoveForce.z = 0.0f;
                                vecEntityMoveForce.x = vecEntityMoveForce.x * 0.5f;
                                vecEntityMoveForce.y = vecEntityMoveForce.y * 0.5f;
                            }
                        }
                        if (bEntityPedIsStandingOnThis)
                        {
                            vecEntityMoveForce.x = vecEntityMoveForce.x + vecEntityMoveForce.x;
                            vecEntityMoveForce.y = vecEntityMoveForce.y + vecEntityMoveForce.y;
                        }

                        physical->ApplyMoveForce(vecEntityMoveForce);
                    }

                    float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass;
                    AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

                    fCollisionImpact1 = entityDamageIntensity / fEntityMass;
                    AudioEngine.ReportCollision(physical, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                    return true;
                }
            }
            else
            {
                CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
                CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

                if (physicalFlags.bAddMovingCollisionSpeed && GetIsTypeVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                    vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

                CVector vecDistanceToPoint = colPoint.m_vecPoint - physical->GetPosition();
                CVector vecEntitySpeed = physical->GetSpeed(vecDistanceToPoint);

                if (physical->physicalFlags.bAddMovingCollisionSpeed && physical->GetIsTypeVehicle() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
                    vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);

                float fThisSpeedDotProduct = DotProduct(vecThisSpeed, colPoint.m_vecNormal);
                float fEntitySpeedDotProduct = DotProduct(vecEntitySpeed, colPoint.m_vecNormal);

                CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
                CVector vecThisCrossProduct = CrossProduct(vecThisDifference, colPoint.m_vecNormal);
                float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
                float fThisCollisionMass = 1.0f / (squaredMagnitude / (fThisMassFactor * m_fTurnMass) + 1.0f / (fThisMassFactor * m_fMass));

                CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
                CVector vecEntityCrossProduct = CrossProduct(vecEntityDifference, colPoint.m_vecNormal);
                squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();
                float fEntityCollisionMass = 1.0f / (squaredMagnitude / (fEntityMassFactor * physical->m_fTurnMass) + 1.0f / (fEntityMassFactor * physical->m_fMass));

                float fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisCollisionMass);
                float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
                if (fThisMoveSpeedDifference < 0.0f)
                {
                    float fThisMoveSpeedElasticity = 0.0f;
                    float fEntityMoveSpeedElasticity = 0.0f;

                    float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                    if (GetHasHitWall())
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;
                    }
                    if (physical->GetHasHitWall())
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fEntityMoveSpeedElasticity = fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
                    }

                    thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

                    CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                    CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;

                    if (GetIsTypeVehicle() && !GetHasHitWall())
                    {
                        if (colPoint.m_vecNormal.z < 0.7f)
                        {
                            vecThisMoveForce.z *= 0.3f;
                        }

                        if (!GetStatus())
                        {
                            vecDistanceToPointFromThis *= 0.8f;
                        }

                        if (CWorld::bNoMoreCollisionTorque)
                        {
                            CVector vecFrictionForce = vecThisMoveForce * -0.3f;
                            ApplyFrictionForce(vecFrictionForce, vecDistanceToPointFromThis);
                        }
                    }

                    if (physical->GetIsTypeVehicle() && !physical->GetHasHitWall())
                    {
                        if ((colPoint.m_vecNormal.z * -1.0f) < 0.7f)
                        {
                            vecEntityMoveForce.z *= 0.3f;
                        }

                        if (!physical->GetStatus())
                        {
                            vecDistanceToPoint *= 0.8f;

                        }
                        if (CWorld::bNoMoreCollisionTorque)
                        {
                            CVector vecFrictionForce = vecEntityMoveForce * -0.3f;
                            physical->ApplyFrictionForce(vecFrictionForce, vecDistanceToPoint);
                        }
                    }
                    if (!physicalFlags.bDisableCollisionForce)
                    {
                        ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
                    }

                    if (physical->physicalFlags.bDisableCollisionForce)
                    {
                        // nothing
                    }
                    else
                    {
                        if (physical->GetIsInSafePosition())
                        {
                            physical->m_vecMoveSpeed *= -1.0f;
                            physical->m_vecTurnSpeed *= -1.0f;
                            physical->ApplySpeed();

                            physical->m_vecMoveSpeed *= -1.0f;
                            physical->m_vecTurnSpeed *= -1.0f;
                            physical->SetIsInSafePosition(false);
                        }

                        physical->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
                    }

                    float fCollisionImpact1 = thisDamageIntensity / fThisCollisionMass;
                    AudioEngine.ReportCollision(this, physical, colPoint.m_nSurfaceTypeA, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);

                    fCollisionImpact1 = entityDamageIntensity / fEntityCollisionMass;
                    AudioEngine.ReportCollision(physical, this, colPoint.m_nSurfaceTypeB, colPoint.m_nSurfaceTypeA, colPoint.m_vecPoint, &colPoint.m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}

// 0x54BA60
bool CPhysical::ProcessCollisionSectorList(int32 sectorX, int32 sectorY)
{
    static auto& colPoints = StaticRef<std::array<CColPoint, 32>>(0xB73710);
    // Como el original: marca la guarda de inicialización del array estático de colpoints
    static auto& colPointsInitGuard = StaticRef<uint32>(0xB73C90);
    if ((colPointsInitGuard & 1) == 0) {
        colPointsInitGuard |= 1;
    }

    // Como el original: CMatrix * CVector (0x59C890) con su orden de sumas
    const auto MultiplyMatrixWithVector = [](const CMatrix& m, const CVector& v) {
        CVector out;
        out.x = ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x;
        out.y = ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y;
        out.z = ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z;
        return out;
    };

    bool bResult = false;

    bool bCollisionDisabled = false;
    bool bCollidedEntityCollisionIgnored = false;
    bool bCollidedEntityUnableToMove = false;
    bool bThisOrCollidedEntityStuck = false;

    float fThisDamageIntensity = -1.0f;
    float fEntityDamageIntensity = -1.0f;
    float fThisMaxDamageIntensity = 0.0f;
    float fEntityMaxDamageIntensity = 0.0f;

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(m_nModelIndex);
    float fBoundingSphereRadius = mi->GetColModel()->GetBoundRadius();

    CVector vecBoundCentre = GetBoundCentre();

    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) {
        auto* const thisObject = AsObject();
        auto* const thisPed = AsPed();
        auto* const thisVehicle = AsVehicle();
        for (auto* const entity : list) {
            auto* const physicalEntity = entity->AsPhysical();
            auto* const entityObject = entity->AsObject();
            auto* const entityPed = entity->AsPed();
            auto* const entityVehicle = entity->AsVehicle();

            if (!entity->m_bUsesCollision || physicalEntity == this || entity->IsScanCodeCurrent()) {
                continue;
            }

            if (!entity->GetIsTouching(vecBoundCentre, fBoundingSphereRadius)) {
                if (m_pEntityIgnoredCollision == entity && static_cast<CEntity*>(m_pAttachedTo) != entity) {
                    m_pEntityIgnoredCollision = nullptr;
                }

                if (entity->GetIsTypePhysical() && physicalEntity->m_pEntityIgnoredCollision == this &&
                    physicalEntity->m_pAttachedTo != this) {
                    physicalEntity->m_pEntityIgnoredCollision = nullptr;
                }
                continue;
            }

            bCollisionDisabled = false;
            bCollidedEntityCollisionIgnored = false;
            bCollidedEntityUnableToMove = false;
            bThisOrCollidedEntityStuck = false;

            physicalFlags.bSkipLineCol = false;

            if (entity->GetIsTypeBuilding()) {
                bCollidedEntityCollisionIgnored = false;
                if (physicalFlags.bInfiniteMass && GetIsStuck()) {
                    bThisOrCollidedEntityStuck = true;
                }

                if (physicalFlags.bDisableCollisionForce && (!GetIsTypeVehicle() || thisVehicle->IsSubTrain())) {
                    bCollisionDisabled = true;
                } else {
                    if (m_pAttachedTo && m_pAttachedTo->GetIsTypePhysical() &&
                        m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                        bCollisionDisabled = true;
                    } else if (m_pEntityIgnoredCollision == entity) {
                        bCollisionDisabled = true;
                    } else if (!physicalFlags.bDisableZ || physicalFlags.bApplyGravity) {
                        if (physicalFlags.bDontCollideWithFlyers) {
                            if (GetStatus()) {
                                if (GetStatus() != STATUS_REMOTE_CONTROLLED && entity->DoesNotCollideWithFlyers()) {
                                    bCollisionDisabled = true;
                                }
                            }
                        }
                    } else {
                        bCollisionDisabled = true;
                    }
                }
            } else {
                SpecialEntityPreCollisionStuff(
                    entity->AsPhysical(),
                    false,
                    bCollisionDisabled,
                    bCollidedEntityCollisionIgnored,
                    bCollidedEntityUnableToMove,
                    bThisOrCollidedEntityStuck
                );
            }

            if (!GetUsesCollision() || bCollidedEntityCollisionIgnored || bCollisionDisabled) {
                entity->SetCurrentScanCode() ;
                if (!bCollisionDisabled) // if collision is enabled then
                {
                    int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                    if (physicalFlags.bForceHitReturnFalse && !bCollidedEntityCollisionIgnored && totalColPointsToProcess > 0) {
                        return true;
                    }
                    if (!totalColPointsToProcess && m_pEntityIgnoredCollision == entity && this == FindPlayerPed()) {
                        m_pEntityIgnoredCollision = nullptr;
                    }
                }
                continue;
            }

            if (entity->GetIsTypeBuilding() || physicalEntity->physicalFlags.bCollidable || bCollidedEntityUnableToMove) {
                entity->SetCurrentScanCode() ;

                int32 totalAcceptableColPoints = 0;
                float fThisMaxDamageIntensity = 0.0f;
                CVector vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                CVector vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);

                int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                if (totalColPointsToProcess > 0) {
                    if (GetHasContacted()) {
                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                                CColPoint* colPoint = &colPoints[colPointIndex];
                                if (bThisOrCollidedEntityStuck || (colPoint->m_nPieceTypeA >= 13 && colPoint->m_nPieceTypeA <= 16)) {
                                    ApplySoftCollision(entity, *colPoint, fThisDamageIntensity);
                                } else if (ApplyCollisionAlt(physicalEntity, *colPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed)) {
                                    ++totalAcceptableColPoints;
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }

                                    if (GetIsTypeVehicle()) {
                                        if (!thisVehicle->IsBoat() || colPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID) {
                                            SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint, 1.0f);
                                        }
                                    } else {
                                        SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint, 1.0f);
                                    }
                                }
                            }
                        }
                    } else {
                        for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                            CColPoint* colPoint = &colPoints[colPointIndex];
                            if (bThisOrCollidedEntityStuck || (colPoint->m_nPieceTypeA >= 13 && colPoint->m_nPieceTypeA <= 16)) {
                                if (ApplySoftCollision(entity, *colPoint, fThisDamageIntensity) &&
                                    (colPoint->m_nSurfaceTypeA != SURFACE_WHEELBASE || colPoint->m_nSurfaceTypeB != SURFACE_WHEELBASE)) {
                                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                    if (ApplyFriction(fSurfaceFriction, *colPoint)) {
                                        SetHasContacted(true);
                                    }
                                    continue;
                                }
                            } else if (ApplyCollisionAlt(physicalEntity, *colPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed)) {
                                ++totalAcceptableColPoints;
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }

                                float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                float fFriction = fSurfaceFriction / totalColPointsToProcess;
                                if (!GetIsTypeVehicle()) {
                                    // Como el original: (150 * f) * daño
                                    fFriction = 150.0f * fFriction * fThisDamageIntensity;
                                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                                    if (ApplyFriction(fFriction, *colPoint)) {
                                        SetHasContacted(true);
                                    }
                                    continue;
                                }
                                if (!thisVehicle->IsBoat() || colPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID) {
                                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                                } else {
                                    fFriction = 0.0f;
                                }

                                // Como el original: los barcos no pasan por el bloque de estado y el tren se mira por el subtipo
                                if (m_nModelIndex == MODEL_RCBANDIT) {
                                    fFriction *= 0.2f;
                                } else if (thisVehicle->IsBoat()) {
                                    if (colPoint->m_vecNormal.z > 0.6f) {
                                        if (g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_LOOSE ||
                                            g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_SAND) {
                                            fFriction *= 3.0f;
                                        }
                                    } else {
                                        fFriction = 0.0f;
                                    }
                                } else if (!thisVehicle->IsSubTrain()) {
                                    if (GetStatus() == STATUS_WRECKED) {
                                        fFriction *= 3.0f;
                                    } else if (GetUp().z > 0.3f && m_vecMoveSpeed.SquaredMagnitude() < 0.02f && m_vecTurnSpeed.SquaredMagnitude() < 0.01f) {
                                        fFriction = 0.0f;
                                    } else if (GetStatus() == STATUS_ABANDONED || DotProduct(&colPoint->m_vecNormal, &GetUp()) < 0.707f) {
                                        // Como el original: con producto escalar NaN no se aplica
                                        fFriction = 150.0f / m_fMass * fFriction * fThisDamageIntensity;
                                    }
                                }

                                if (GetIsTypeVehicle() && thisVehicle->IsSubTrain()) {
                                    fFriction = fFriction + fFriction;
                                }
                                if (ApplyFriction(fFriction, *colPoint)) {
                                    SetHasContacted(true);
                                }
                            }
                        }
                    }
                }

                if (totalAcceptableColPoints) {
                    float fSpeedFactor = 1.0f / totalAcceptableColPoints;
                    m_vecMoveSpeed += vecMoveSpeed * fSpeedFactor;
                    m_vecTurnSpeed += vecTurnSpeed * fSpeedFactor;
                    if (!CWorld::bNoMoreCollisionTorque) {
                        if (TreatAsPlayerForCollisions() && GetIsTypeVehicle()) {
                            float fThisMoveSpeedX = m_vecMoveSpeed.x;
                            if (m_vecMoveSpeed.x < 0.0f) {
                                fThisMoveSpeedX = -fThisMoveSpeedX;
                            }

                            if (fThisMoveSpeedX > 0.2f) {
                                float fThisMoveSpeedY = m_vecMoveSpeed.y;
                                if (m_vecMoveSpeed.y < 0.0f) {
                                    fThisMoveSpeedY = -fThisMoveSpeedY;
                                }
                                if (fThisMoveSpeedY > 0.2f) {
                                    if (!physicalFlags.bSubmergedInWater) {
                                        m_vecFrictionMoveSpeed.x -= vecMoveSpeed.x * 0.3f / (float)totalColPointsToProcess;
                                        m_vecFrictionMoveSpeed.y -= vecMoveSpeed.y * 0.3f / (float)totalColPointsToProcess;
                                        // Como el original: CVector / float (0x4119D0) multiplica por el inverso
                                        const float fInvColPoints = 1.0f / (float)totalColPointsToProcess;
                                        const CVector vecTurnFriction = vecTurnSpeed * -0.3f;
                                        m_vecFrictionTurnSpeed.x += vecTurnFriction.x * fInvColPoints;
                                        m_vecFrictionTurnSpeed.y += vecTurnFriction.y * fInvColPoints;
                                        m_vecFrictionTurnSpeed.z += vecTurnFriction.z * fInvColPoints;
                                    }
                                }
                            }
                        }
                    }

                    if (entity->GetIsTypeObject() && entityObject->m_nColDamageEffect && fThisMaxDamageIntensity > 20.0f) {
                        entityObject->ObjectDamage(fThisMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, this, WEAPON_UNIDENTIFIED);
                    }
                    if (!CWorld::bSecondShift) {
                        return true;
                    }

                    bResult = true;
                }
            } else {
                entity->SetCurrentScanCode() ;

                int32 totalAcceptableColPoints = 0;
                int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                if (totalColPointsToProcess <= 0)
                    continue;

                fThisMaxDamageIntensity = 0.0f;
                fEntityMaxDamageIntensity = 0.0f;
                if (GetHasContacted() && entity->GetHasContacted()) {
                    if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex4 = 0; colPointIndex4 < totalColPointsToProcess; colPointIndex4++) {
                            CColPoint* colPoint2 = &colPoints[colPointIndex4];
                            if (bThisOrCollidedEntityStuck || (colPoint2->m_nPieceTypeA >= 13 && colPoint2->m_nPieceTypeA <= 16) ||
                                (colPoint2->m_nPieceTypeB >= 13 && colPoint2->m_nPieceTypeB <= 16)) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity);
                                // Como el original: también registra la pieza dañada de la entidad si la pieza B es 13..16
                                if (colPoint2->m_nPieceTypeB >= 13 && colPoint2->m_nPieceTypeB <= 16) {
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint2, -1.0f);
                                }
                            } else {
                                if (ApplyCollision(entity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }
                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint2, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint2, -1.0f);
                                }
                            }
                        }
                    }
                } else if (GetHasContacted()) {
                    SetHasContacted(false);
                    CVector vecThisFrictionMoveSpeed = m_vecFrictionMoveSpeed;
                    CVector vecThisFrictionTurnSpeed = m_vecFrictionTurnSpeed;
                    ResetFrictionMoveSpeed();
                    ResetFrictionTurnSpeed();
                    if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex1 = 0; colPointIndex1 < totalColPointsToProcess; colPointIndex1++) {
                            CColPoint* colPoint1 = &colPoints[colPointIndex1];
                            if (bThisOrCollidedEntityStuck || (colPoint1->m_nPieceTypeA >= 13 && colPoint1->m_nPieceTypeA <= 16) ||
                                (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16)) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity);
                                if (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16) {
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);
                                }
                            } else if (ApplyCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity)) {
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }
                                if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                    fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                }

                                SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint1, 1.0f);
                                physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);

                                float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint1);
                                float fFriction3 = fSurfaceFriction1 / totalColPointsToProcess;
                                if (GetIsTypeVehicle() && entity->GetIsTypeVehicle() &&
                                    (m_vecMoveSpeed.SquaredMagnitude() > 0.02f || m_vecTurnSpeed.SquaredMagnitude() > 0.01f)) {
                                    fFriction3 *= 1.0f * fThisDamageIntensity;
                                }

                                if (entity->GetIsStatic()) {
                                    if (ApplyFriction(fFriction3, *colPoint1)) {
                                        SetHasContacted(true);
                                    }
                                } else if (ApplyFriction(physicalEntity, fFriction3, *colPoint1)) {
                                    SetHasContacted(true);
                                    entity->SetHasContacted(true);
                                }
                            }
                        }
                    }

                    if (!GetHasContacted()) {
                        SetHasContacted(true);
                        m_vecFrictionMoveSpeed = vecThisFrictionMoveSpeed;
                        m_vecFrictionTurnSpeed = vecThisFrictionTurnSpeed;
                    }
                } else {
                    if (entity->GetHasContacted()) {
                        entity->SetHasContacted(false);
                        CVector vecEntityMoveSpeed = physicalEntity->m_vecFrictionMoveSpeed;
                        CVector vecEntityFrictionTurnSpeed = physicalEntity->m_vecFrictionTurnSpeed;
                        physicalEntity->ResetFrictionMoveSpeed();
                        physicalEntity->ResetFrictionTurnSpeed();

                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex3 = 0; colPointIndex3 < totalColPointsToProcess; colPointIndex3++) {
                                CColPoint* colPoint4 = &colPoints[colPointIndex3];
                                if (bThisOrCollidedEntityStuck || (colPoint4->m_nPieceTypeA >= 13 && colPoint4->m_nPieceTypeA <= 16) ||
                                    (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16)) {
                                    ++totalAcceptableColPoints;
                                    ApplySoftCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity);
                                    if (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16) {
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);
                                    }
                                } else if (ApplyCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }

                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint4, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);

                                    float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint4);

                                    float fFriction2 = fSurfaceFriction1 / totalColPointsToProcess;
                                    if (GetIsTypeVehicle() && entity->GetIsTypeVehicle() &&
                                        (m_vecMoveSpeed.SquaredMagnitude() > 0.02f || m_vecTurnSpeed.SquaredMagnitude() > 0.01f)) {
                                        fFriction2 *= 1.0f * fThisDamageIntensity;
                                    }

                                    if (entity->GetIsStatic()) {
                                        if (ApplyFriction(fFriction2, *colPoint4)) {
                                            SetHasContacted(true);
                                        }
                                    } else if (ApplyFriction(physicalEntity, fFriction2, *colPoint4)) {
                                        SetHasContacted(true);
                                        entity->SetHasContacted(true);
                                    }
                                }
                            }
                        }

                        if (!entity->GetHasContacted()) {
                            entity->SetHasContacted(true);
                            physicalEntity->m_vecFrictionMoveSpeed = vecEntityMoveSpeed;
                            physicalEntity->m_vecFrictionTurnSpeed = vecEntityFrictionTurnSpeed;
                        }
                    } else if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex2 = 0; colPointIndex2 < totalColPointsToProcess; colPointIndex2++) {
                            CColPoint* colPoint3 = &colPoints[colPointIndex2];
                            if (bThisOrCollidedEntityStuck || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16)
                                // || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16) // BUG: I think it should be m_nPieceTypeB
                            ) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity);
                                if (colPoint3->m_nPieceTypeB >= 13 && colPoint3->m_nPieceTypeB <= 16) {
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);
                                }
                            } else if (ApplyCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity)) {
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }

                                if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                    fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                }

                                SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint3, 1.0f);
                                physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);

                                float fSurfaceFirction2 = g_surfaceInfos.GetAdhesiveLimit(colPoint3);
                                float fFriction1 = fSurfaceFirction2 / totalColPointsToProcess;
                                if (GetIsTypeVehicle() && entity->GetIsTypeVehicle() &&
                                    (m_vecMoveSpeed.SquaredMagnitude() > 0.02f || m_vecTurnSpeed.SquaredMagnitude() > 0.01f)) {
                                    fFriction1 *= 1.0f * fThisDamageIntensity;
                                }

                                if (entity->GetIsStatic()) {
                                    if (ApplyFriction(fFriction1, *colPoint3)) {
                                        SetHasContacted(true);
                                    }
                                } else if (ApplyFriction(physicalEntity, fFriction1, *colPoint3)) {
                                    SetHasContacted(true);
                                    entity->SetHasContacted(true);
                                }
                            }
                        }
                    }
                }

                if (entity->GetIsTypePed() && GetIsTypeVehicle()) {
                    float fThisMoveSpeedDot = m_vecMoveSpeed.SquaredMagnitude();
                    // Como el original: se comprueba el bit 12 de los flags de la entidad (m_bHasHitWall)
                    if (!entityPed->IsPlayer() || entity->m_bHasHitWall && m_vecMoveSpeed.SquaredMagnitude() > 0.0025f) {
                        entityPed->KillPedWithCar(AsVehicle(), fEntityMaxDamageIntensity, false);
                    }
                } else if (GetIsTypePed() && entity->GetIsTypeVehicle() && entityVehicle->IsSubTrain() &&
                            (DotProduct(&entityVehicle->m_vecMoveSpeed, &m_vecLastCollisionImpactVelocity) > 0.2f ||
                            thisPed->bFallenDown && entityVehicle->m_vecMoveSpeed.SquaredMagnitude() > 0.0005f)) {
                    float fDamageIntensity = fThisMaxDamageIntensity + fThisMaxDamageIntensity;
                    thisPed->KillPedWithCar(entityVehicle, fDamageIntensity, false);
                } else if (entity->GetIsTypeObject() && GetIsTypeVehicle() && entity->m_bUsesCollision) {
                    if (entityObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0f) {
                        entityObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, this, WEAPON_RUNOVERBYCAR);
                    } else {
                        if (entityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY) {
                            CBaseModelInfo* pEntityModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                            CColModel* colModel = pEntityModelInfo->GetColModel();
                            // Como el original: m_matrix directo y CMatrix * CVector (0x59C890)
                            CVector boundBoxPos = MultiplyMatrixWithVector(*entity->m_matrix, colModel->m_boundBox.GetSize());

                            bool bObjectDamage = false;
                            if (GetPosition().z > boundBoxPos.z) {
                                bObjectDamage = true;
                            } else {
                                CMatrix invertedMatrix;
                                invertedMatrix = Invert(*m_matrix, invertedMatrix);
                                if (MultiplyMatrixWithVector(invertedMatrix, boundBoxPos).z < 0.0f)
                                    bObjectDamage = true;
                            }
                            if (bObjectDamage)
                                entityObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, this, WEAPON_RUNOVERBYCAR);
                        }
                    }
                } else if (GetIsTypeObject() && entity->GetIsTypeVehicle() && GetUsesCollision()) {
                    if (thisObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0f) {
                        thisObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, entity, WEAPON_RUNOVERBYCAR);
                    } else {
                        // BUG: entity is a vehicle here, but we are treating it as an object?
                        if (entityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY) {

                            CColModel* colModel = mi->GetColModel();
                            // Como el original: CMatrix * CVector (0x59C890)
                            CVector boundBoxPos = MultiplyMatrixWithVector(*m_matrix, colModel->m_boundBox.GetSize());

                            bool bObjectDamage = false;
                            if (boundBoxPos.z < entity->GetPosition().z) {
                                bObjectDamage = true;
                            } else {
                                CMatrix invertedMatrix;
                                invertedMatrix = Invert(*entity->m_matrix, invertedMatrix);
                                if (MultiplyMatrixWithVector(invertedMatrix, boundBoxPos).z < 0.0f)
                                    bObjectDamage = true;
                            }

                            if (bObjectDamage)
                                thisObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, entity, WEAPON_RUNOVERBYCAR);
                        }
                    }
                }

                if (entity->GetStatus() == STATUS_SIMPLE) {
                    entity->SetStatus(STATUS_PHYSICS);
                    if (entity->GetIsTypeVehicle()) {
                        CCarCtrl::SwitchVehicleToRealPhysics(entityVehicle);
                    }
                }
                if (CWorld::bSecondShift) {
                    bResult = true;
                } else if (totalColPointsToProcess > totalAcceptableColPoints) {
                    return true;
                }
            }
        }
        return false;
    };
    // Como el original: el sector repetido usa `& 15` (también con coordenadas negativas)
    auto& s = CWorld::GetSector(sectorX, sectorY);
    auto& rs = CWorld::ms_aRepeatSectors[(uint32)sectorY & (MAX_REPEAT_SECTORS_Y - 1)][(uint32)sectorX & (MAX_REPEAT_SECTORS_X - 1)];
    // Como el original: orden edificios, vehículos, objetos, peds
    if (   ProcessSectorList(s.Buildings)
        || ProcessSectorList(rs.Vehicles)
        || ProcessSectorList(rs.Objects)
        || ProcessSectorList(rs.Peds)
    ) {
        return true;
    }
    return bResult;
}

// 0x54CFF0
bool CPhysical::ProcessCollisionSectorList_SimpleCar(CRepeatSector* repeatSector)
{
    static auto& colPoints = StaticRef<std::array<CColPoint, 32>>(0xB73C98);
    // Como el original: marca la guarda de inicialización del array estático de colpoints
    static auto& colPointsInitGuard = StaticRef<uint32>(0xB74218);
    if ((colPointsInitGuard & 1) == 0) {
        colPointsInitGuard |= 1;
    }
    float fThisDamageIntensity = -1.0f;
    float fEntityDamageIntensity = -1.0f;

    if (!GetUsesCollision())
    {
        return false;
    }

    CVector vecBoundingCentre = GetBoundCentre();

    float fBoundingRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();

    int32 totalColPointsToProcess = 0;
    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) -> CPhysical* {
        for (auto* const entity : list) {
            bool isLampTouchingGround = false;
            // Como el original: GetMatrix() (crea la matriz si no existe) y up.z < 0.66
            if (entity->GetIsTypeObject() && entity->AsObject()->objectFlags.bIsLampPost && entity->GetMatrix().GetUp().z < 0.66f)
                isLampTouchingGround = true;

            if (entity != this
                && !isLampTouchingGround
                && !entity->IsScanCodeCurrent())
            {
                if (entity->m_bUsesCollision && entity->GetIsTouching(vecBoundingCentre, fBoundingRadius)) {
                    entity->SetCurrentScanCode() ;
                    totalColPointsToProcess = ProcessEntityCollision(entity, &colPoints[0]);
                    if (totalColPointsToProcess > 0) {
                        return entity;
                    }
                }
            }
        }
        return nullptr;
    };

    // Find entity we're colliding with
    CPhysical* entity;
    // Como el original: primero objetos y luego vehículos
    if (!(entity = ProcessSectorList(repeatSector->Objects))) {
        if (!(entity = ProcessSectorList(repeatSector->Vehicles))) {
            return false;
        }
    }

    assert(entity);
    if (GetHasContacted() && entity->GetHasContacted())
    {
        if (totalColPointsToProcess > 0)
        {
            for( int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
            {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity))
                {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                }
            }
        }
    }
    else if (GetHasContacted())
    {
        CVector vecOldFrictionMoveSpeed = m_vecFrictionMoveSpeed;
        CVector vecOldFrictionTurnSpeed = m_vecFrictionTurnSpeed;
        ResetFrictionTurnSpeed();
        ResetFrictionMoveSpeed();
        SetHasContacted(false);

        if (totalColPointsToProcess > 0)
        {
            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
            {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity))
                {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                    if (ApplyFriction(entity, fFriction, *colPoint))
                    {
                        SetHasContacted(true);
                        entity->SetHasContacted(true);
                    }
                }
            }
        }

        if (!GetHasContacted())
        {
            m_vecFrictionMoveSpeed = vecOldFrictionMoveSpeed;
            m_vecFrictionTurnSpeed = vecOldFrictionTurnSpeed;
            SetHasContacted(true);
        }
    }
    else
    {
        if (entity->GetHasContacted())
        {
            assert(entity);
            CVector vecOldFrictionMoveSpeed = entity->m_vecFrictionMoveSpeed;
            CVector vecOldFrictionTurnSpeed = entity->m_vecFrictionTurnSpeed;
            entity->ResetFrictionTurnSpeed();
            entity->ResetFrictionMoveSpeed();
            entity->SetHasContacted(false);

            if (totalColPointsToProcess > 0)
            {
                for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                {
                    CColPoint* colPoint = &colPoints[colPointIndex];
                    if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity))
                    {
                        SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                        entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                        float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                        float fFriction = fSurfaceFriction / totalColPointsToProcess;
                        if (ApplyFriction(entity, fFriction, *colPoint))
                        {
                            SetHasContacted(true);
                            entity->SetHasContacted(true);
                        }
                    }
                }
            }
            if (!entity->GetHasContacted())
            {
                entity->m_vecFrictionMoveSpeed = vecOldFrictionMoveSpeed;
                entity->m_vecFrictionTurnSpeed = vecOldFrictionTurnSpeed;
                entity->SetHasContacted(true);
            }
        }
        else if (totalColPointsToProcess > 0)
        {
            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
            {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity))
                {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                    if (ApplyFriction(entity, fFriction, *colPoint))
                    {
                        SetHasContacted(true);
                        entity->SetHasContacted(true);
                    }
                }
            }
        }
    }

    if (entity->GetStatus() == STATUS_SIMPLE)
    {
        entity->SetStatus(STATUS_PHYSICS);
        if (entity->GetIsTypeVehicle())
        {
            CCarCtrl::SwitchVehicleToRealPhysics(entity->AsVehicle());
        }
    }
    return true;
}

// 0x54D570
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector vecAttachOffset, CVector vecAttachRotation)
{
    if (!entityAttachTo)
        return;

    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    m_vecAttachOffset = vecAttachOffset;
    if (physicalFlags.bInfiniteMass)
        m_vecAttachedEntityRotation = GetPosition();
    else
        m_vecAttachedEntityRotation = vecAttachRotation;
    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    m_pEntityIgnoredCollision = m_pAttachedTo; // Como el original: se ignora la entidad recién enganchada (no la anterior)
    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
        PositionAttachedEntity();
    }
    else {
        if (m_pAttachedTo->GetIsTypePhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && GetIsTypeObject() && !physicalFlags.bInfiniteMass)
        {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
        PositionAttachedEntity();
    }
}

// 0x54D690
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector* vecAttachOffset, CQuaternion* attachRotation)
{
    if (!entityAttachTo)
        return;

    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    CMatrix entityAttachedtoMatrix(m_pAttachedTo->GetMatrix());
    auto* attachedToAutoMobile = m_pAttachedTo->AsAutomobile();
    if (GetIsTypeObject() && m_pAttachedTo->m_nModelIndex == MODEL_FORKLIFT)
    {
        if (RwFrame* carMiscAFrame = attachedToAutoMobile->m_aCarNodes[CAR_MISC_A])
            entityAttachedtoMatrix.UpdateMatrix(RwFrameGetLTM(carMiscAFrame));
    }

    if (vecAttachOffset) {
        m_vecAttachOffset = *vecAttachOffset;
    }
    else {
        CVector distance = GetPosition() - entityAttachedtoMatrix.GetPosition();
        // Como el original: orden de sumas del exe (x: y+z primero; y/z: x+y+z)
        const CVector& r = entityAttachedtoMatrix.GetRight();
        const CVector& f = entityAttachedtoMatrix.GetForward();
        const CVector& u = entityAttachedtoMatrix.GetUp();
        m_vecAttachOffset.x = (r.y * distance.y + r.z * distance.z) + distance.x * r.x;
        m_vecAttachOffset.y = (f.x * distance.x + f.y * distance.y) + f.z * distance.z;
        m_vecAttachOffset.z = (u.x * distance.x + u.y * distance.y) + u.z * distance.z;
    }

    if (attachRotation) {
        m_qAttachedEntityRotation = *attachRotation;
    }
    else {
        CMatrix entityAttachedToMatrixInverted;
        Invert(entityAttachedtoMatrix, entityAttachedToMatrixInverted);
        entityAttachedToMatrixInverted *= *m_matrix;
        RwMatrixTag rwMatrix{};
        entityAttachedToMatrixInverted.CopyToRwMatrix(&rwMatrix);
        RtQuatConvertFromMatrix((RtQuat*)&m_qAttachedEntityRotation, &rwMatrix);
    }

    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
    }
    else {
        if (m_pAttachedTo->GetIsTypePhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && GetIsTypeObject() && !physicalFlags.bInfiniteMass)
        {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
    }

    if (physicalFlags.bInfiniteMass)
        m_vecAttachedEntityRotation = GetPosition();

    m_pEntityIgnoredCollision = m_pAttachedTo; // Como el original: se ignora la entidad recién enganchada (no la anterior)
    PositionAttachedEntity();
}

// 0x54D920
bool CPhysical::CheckCollision()
{
    SetCollisionProcessed(false);
    if (GetIsTypePed())
    {
        CPed* ped = AsPed();
        if (!m_pAttachedTo && !physicalFlags.bForceHitReturnFalse && !physicalFlags.bProcessingShift && !physicalFlags.bSkipLineCol) {
            ped->m_standingOnEntity = nullptr;
            if (ped->bIsStanding) {
                ped->bIsStanding = false;
                ped->bWasStanding = true;
            }
        }

        if (ped->IsPlayer()) {
            CTaskSimpleClimb* taskClimb = ped->GetIntelligence()->GetTaskClimb();
            if (taskClimb) {
                switch (taskClimb->GetHeightForPos()) {
                case CLIMB_GRAB:
                case CLIMB_PULLUP:
                case CLIMB_STANDUP:
                case CLIMB_VAULT: physicalFlags.bSkipLineCol = true; break;
                }
            }
        }
    }

    CWorld::AdvanceCurrentScanCode();

    CRect boundRect = GetBoundRect();
    // Como el original: floor(x * 0.02 + 60) con el recíproco en x87 (no divide por 50)
    int32 startSectorX = PhysicalSector(boundRect.left);
    int32 startSectorY = PhysicalSector(boundRect.bottom);
    int32 endSectorX = PhysicalSector(boundRect.right);
    int32 endSectorY = PhysicalSector(boundRect.top);
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            if (ProcessCollisionSectorList(sectorX, sectorY))
                return true;
        }
    }
    return false;
}

// 0x54DAB0
bool CPhysical::CheckCollision_SimpleCar()
{
    SetCollisionProcessed(false);
    CWorld::AdvanceCurrentScanCode();
    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    if (!entryInfoNode)
        return false;

    while (!ProcessCollisionSectorList_SimpleCar(entryInfoNode->m_repeatSector))
    {
        entryInfoNode = entryInfoNode->m_next;
        if (!entryInfoNode)
            return false;
    }
    return true;
}
