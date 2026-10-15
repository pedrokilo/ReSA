/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CObject
 * Reconstrucción: COMPLETA — 48/48 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 48/48 entradas.
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

#include "Object.h"
#include "Radar.h"
#include "BreakManager_c.h"
#include "Buoyancy.h"
#include "ObjectSaveStructure.h"
#include "Rope.h"
#include "Ropes.h"
#include "TheScripts.h"
#include "Shadows.h"
#include "Garages.h"
#include "CustomBuildingRenderer.h"
#include "CustomBuildingDNPipeline.h"
#include "X87Math.h"

namespace {
// Como el original: constantes float exactas del EXE
constexpr float ObjF(uint32 bits) { return std::bit_cast<float>(bits); }
const float OBJ_PI       = ObjF(0x40490FDBu); // 0x858CB8
const float OBJ_TWO_PI   = ObjF(0x40C90FDBu); // 0x858CBC
const float OBJ_MINUS_PI = ObjF(0xC0490FDBu); // 0x858CC0
const float OBJ_HALF_PI  = ObjF(0x3FC90FDBu); // 0x858FE4

// Como el original: variables globales del EXE usadas por las puertas y los pasos de colisión
auto& fObjectStepSizeMult = StaticRef<float>(0x8D0A28); // 0.5
auto& fDoorSpeedMult      = StaticRef<float>(0x8D0A2C); // 0.002
auto& fDoorCutoffSpeed    = StaticRef<float>(0x8D0A30); // 0.02
auto& fMaxDoorDiff        = StaticRef<float>(0x8D0A34); // 0.3

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
CVector ObjMultiply3x3(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: MultiplyMatrixWithVector (0x59C890), con el orden de sumas del EXE
CVector ObjMultiplyPoint(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Como el original: CVector::SquaredMagnitude (0x406DA0), (x·x + y·y) + z·z
float ObjSqMag(const CVector& v) {
    return (v.x * v.x + v.y * v.y) + v.z * v.z;
}

// Como el original: _ftol2 (0x821B40) trunca a 64 bits y se usa la parte baja
int32 ObjFtol(float f) {
    return static_cast<int32>(static_cast<int64>(f));
}

// Como el original: ceil del CRT sobre el valor convertido a double y _ftol2 (0x823820/0x538AD0 + 0x821B40)
uint8 ObjCeilSteps(float f) {
    return static_cast<uint8>(static_cast<int64>(std::ceil(static_cast<double>(f))));
}
} // namespace

void CObject::InjectHooks()
{
    RH_ScopedVirtualClass(CObject, 0x866F60, 23);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedVMTInstall(SetIsStatic, 0x5A0760);
    RH_ScopedVMTInstall(CreateRwObject, 0x59F110);
    RH_ScopedVMTInstall(ProcessControl, 0x5A2130);
    RH_ScopedVMTInstall(Teleport, 0x5A17B0);
    RH_ScopedVMTInstall(PreRender, 0x59FD50);
    RH_ScopedVMTInstall(Render, 0x59F180);
    RH_ScopedVMTInstall(SetupLighting, 0x554FA0);
    RH_ScopedVMTInstall(RemoveLighting, 0x553E10);
    RH_ScopedVMTInstall(SpecialEntityPreCollisionStuff, 0x59FEE0);
    RH_ScopedVMTInstall(SpecialEntityCalcCollisionSteps, 0x5A02E0);
    RH_ScopedInstall(Init, 0x59F840);
    RH_ScopedInstall(ProcessGarageDoorBehaviour, 0x44A4D0);
    RH_ScopedInstall(CanBeDeleted, 0x59F120);
    RH_ScopedInstall(SetRelatedDummy, 0x59F160);
    RH_ScopedInstall(TryToExplode, 0x59F2D0);
    RH_ScopedInstall(SetObjectTargettable, 0x59F300);
    RH_ScopedInstall(CanBeTargetted, 0x59F320);
    RH_ScopedInstall(RefModelInfo, 0x59F330);
    RH_ScopedInstall(SetRemapTexture, 0x59F350);
    RH_ScopedInstall(GetRopeHeight, 0x59F380);
    RH_ScopedInstall(SetRopeHeight, 0x59F3A0);
    RH_ScopedInstall(GetObjectCarriedWithRope, 0x59F3C0);
    RH_ScopedInstall(ReleaseObjectCarriedWithRope, 0x59F3E0);
    RH_ScopedInstall(AddToControlCodeList, 0x59F400);
    RH_ScopedInstall(RemoveFromControlCodeList, 0x59F450);
    RH_ScopedInstall(ResetDoorAngle, 0x59F4B0);
    RH_ScopedInstall(LockDoor, 0x59F5C0);
    RH_ScopedInstall(DoBurnEffect, 0x59FB50);
    RH_ScopedInstall(GetLightingFromCollisionBelow, 0x59FD00);
    RH_ScopedInstall(ProcessSamSiteBehaviour, 0x5A07D0);
    RH_ScopedInstall(ProcessTrainCrossingBehaviour, 0x5A0B50);
    RH_ScopedInstall(ObjectDamage, 0x5A0D90);
    RH_ScopedInstall(Explode, 0x5A1340);
    RH_ScopedInstall(ObjectFireDamage, 0x5A1580);
    RH_ScopedInstall(GrabObjectToCarryWithRope, 0x5A1AB0);
    RH_ScopedInstall(CanBeUsedToTakeCoverBehind, 0x5A1B60);
    RH_ScopedInstall(ProcessControlLogic, 0x5A29A0);
    RH_ScopedOverloadedInstall(Create, "intbool", 0x5A1F60, CObject*(*)(int32, bool));
    RH_ScopedOverloadedInstall(Create, "dummy", 0x5A2070, CObject*(*)(CDummyObject*));
    RH_ScopedInstall(SetMatrixForTrainCrossing, 0x59F200);
    RH_ScopedInstall(TryToFreeUpTempObjects, 0x5A1840);
    RH_ScopedInstall(DeleteAllTempObjects, 0x5A18B0);
    RH_ScopedInstall(DeleteAllMissionObjects, 0x5A1910);
    RH_ScopedInstall(DeleteAllTempObjectsInArea, 0x5A1980);
    RH_ScopedGlobalInstall(IsObjectPointerValid_NotInWorld, 0x5A2B90);
    RH_ScopedGlobalInstall(IsObjectPointerValid, 0x5A2C20);

    RH_ScopedGlobalInstall(Save, 0x5D2830);
    RH_ScopedGlobalInstall(Load, 0x5D2870);
}

// 0x5A1D10
CObject::CObject() : CPhysical() {
    m_pDummyObject = nullptr;
    Init();
    m_nObjectType = eObjectType::OBJECT_UNKNOWN;
}

// 0x5A1D70
CObject::CObject(int32 modelId, bool bCreate) : CPhysical() {
    m_pDummyObject = nullptr;
    if (bCreate)
        CEntity::SetModelIndex(modelId);
    else
        CEntity::SetModelIndexNoCreate(modelId);

    Init();
}

// 0x5A1DF0
CObject::CObject(CDummyObject* dummyObj) : CPhysical() {
    CEntity::SetModelIndexNoCreate(dummyObj->m_nModelIndex);
    if (dummyObj->GetRwObject())
        CEntity::AttachToRwObject(dummyObj->GetRwObject(), true);
    else
        CPlaceable::SetMatrix(dummyObj->GetMatrix());

    dummyObj->DetachFromRwObject();
    Init();

    SetIplIndex(dummyObj->GetIplIndex());
    SetAreaCode(dummyObj->GetAreaCode());
    m_bRenderDamaged = dummyObj->m_bRenderDamaged;

    if (auto* const atomic = GetRpAtomicOrFirstAtomicOfClump()) {
        if (!CCustomBuildingRenderer::IsCBPCPipelineAttached(atomic)) {
            m_bLightObject = true;
        }
    }
}

// 0x59F660
// Libera referencias a script brains, modelos y texturas; retira registros del
// radar y de listas de control, y extingue el fuego antes de liberar el objeto.
CObject::~CObject() {
    if (objectFlags.b0x100000_0x200000) {
        const auto iIndex = SCMToModelId(CTheScripts::ScriptsForBrains.m_aScriptForBrains[m_nStreamedScriptBrainToLoad].m_StreamedScriptIndex);
        CStreaming::SetMissionDoesntRequireModel(iIndex);
        objectFlags.b0x100000_0x200000 = 0;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(this, m_nStreamedScriptBrainToLoad);
        m_nStreamedScriptBrainToLoad = -1;
    }

    if (objectFlags.bHasNoModel) {
        auto* const colModel = CModelInfo::GetModelInfo((m_nModelIndex))->GetColModel();
        CColStore::RemoveRef(colModel->m_nColSlot);
    }

    CRadar::ClearBlipForEntity(eBlipType::BLIP_OBJECT, GetObjectPool()->GetRef(this));

    if (m_nRefModelIndex != -1)
        CModelInfo::GetModelInfo(m_nRefModelIndex)->RemoveRef();

    if (m_wRemapTxd != -1)
        CTxdStore::RemoveRef(m_wRemapTxd);

    if (IsTemporary() && nNoTempObjects)
        --nNoTempObjects;

    RemoveFromControlCodeList();
    if (m_nModelIndex == ModelIndices::MI_TRAINCROSSING1) {
        const auto& p = m_pDummyObject->GetPosition();
        ThePaths.SetLinksBridgeLights(p.x - 12.0F, p.x + 12.0F, p.y - 12.0F, p.y + 12.0F, false);
    }

    if (m_pFire)
        m_pFire->Extinguish();
}

// 0x5A1EE0
void* CObject::operator new(size_t size) {
    return GetObjectPool()->New();
}

// 0x5A1EF0
void* CObject::operator new(size_t size, int32 poolRef) {
    return GetObjectPool()->NewAt(poolRef);
}

// 0x5A1F20
void CObject::operator delete(void* obj) {
    GetObjectPool()->Delete(static_cast<CObject*>(obj));
}

// NOTSA
void CObject::operator delete(void* obj, int32 poolRef) {
    GetObjectPool()->Delete(static_cast<CObject*>(obj));
}

// 0x5A0760
void CObject::SetIsStatic(bool isStatic) {
    CEntity::SetIsStatic(isStatic);
    physicalFlags.bDoorHitEndStop = false;
    if (!isStatic && (physicalFlags.bDisableMoveForce && m_fDoorStartAngle < -1000.0F)) {
        m_fDoorStartAngle = GetHeading();
    }
}

// 0x59F110
void CObject::CreateRwObject() {
    CEntity::CreateRwObject();
}

// 0x5A2130
void CObject::ProcessControl() {
    auto* mi = GetModelInfo();
    auto bIsAnimated = false;
    if (mi->GetRwModelType() == rpCLUMP && mi->bHasAnimBlend && GetRwObject())
        bIsAnimated = true;

    if (m_fDamageIntensity > 0.0F
        && objectFlags.bDamaged
        && !m_pAttachedTo
        && !IsCraneMovingPart()
        && !physicalFlags.bInfiniteMass
        && !physicalFlags.bDisableMoveForce
        && m_pDamageEntity
        // Como el original: el volquete siempre; la carretilla solo con la horquilla levantada
        && (m_pDamageEntity->m_nModelIndex == MODEL_DUMPER
            || m_pDamageEntity->m_nModelIndex == MODEL_FORKLIFT && m_pDamageEntity->AsAutomobile()->m_wMiscComponentAngle)
        && !CRopes::IsCarriedByRope(this)
    ) {
        if (m_nColDamageEffect != COL_DAMAGE_EFFECT_NONE && m_fDamageIntensity > 5.0F)
            ObjectDamage(m_fDamageIntensity, &m_vecLastCollisionPosn, &m_vecLastCollisionImpactVelocity, m_pDamageEntity, eWeaponType::WEAPON_RAMMEDBYCAR);

        if (m_vecLastCollisionImpactVelocity.z > 0.3F) {
            m_nFakePhysics = 0;
            auto* const damageEntity = m_pDamageEntity->AsPhysical();
            const auto vecColDir = GetPosition() - damageEntity->GetPosition();
            const auto vecEntSpeed = damageEntity->GetSpeed(vecColDir);
            const auto vecSpeedDiff = vecEntSpeed - m_vecMoveSpeed;
            if (ObjSqMag(vecSpeedDiff) < 0.0001F && ObjSqMag(m_vecTurnSpeed) < 0.0001F) {
                CPhysical::AttachEntityToEntity(damageEntity, nullptr, nullptr);
                m_fElasticity = 0.2F;
            }
        }
    }

    objectFlags.bDamaged = false;
    if (!GetIsStuck() && !GetIsStatic()) {
        if (!physicalFlags.bDisableZ && !physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
            // Como el original: (a + b) · (1/2) con 0x40FE30 y 0x4119D0
            m_vecForce = (m_vecForce + m_vecMoveSpeed) * 0.5F;
            m_vecTorque = (m_vecTorque + m_vecTurnSpeed) * 0.5F;

            const auto fTimeStep = CTimer::GetTimeStep() * 0.003F;
            // Como el original: NaN reinicia el contador
            if (!(ObjSqMag(m_vecForce) < fTimeStep * fTimeStep) || !(ObjSqMag(m_vecTorque) < fTimeStep * fTimeStep)) {
                m_nFakePhysics = 0;
            } else {
                m_nFakePhysics++;
                if (m_nFakePhysics > 10 && !physicalFlags.bAttachedToEntity) {
                    m_nFakePhysics = 10;
                    if (!bIsAnimated) {
                        SetIsStatic(true);
                    }
                    ResetMoveSpeed();
                    ResetTurnSpeed();
                    ResetFrictionMoveSpeed();
                    ResetFrictionTurnSpeed();
                    return;
                }
            }
        }
    }

    if (!GetIsStatic())
        CPhysical::ProcessControl();

    if (bIsAnimated)
        SetIsStatic(false);

    CVector vecBuoyancyTurnPoint, vecBuoyancyForce;
    if (mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce))
    {
        physicalFlags.bTouchingWater = true;
        physicalFlags.bSubmergedInWater = true;
        SetIsStatic(false);

        CPhysical::ApplyMoveForce(vecBuoyancyForce);
        CPhysical::ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);
        // Como el original: un solo pow(0.97, paso) del CRT (0x822130) con ST0 sin redondear en los seis productos
        // (0x44..0x58); el resultado es determinista, así que dos llamadas por vector dan el mismo factor.
        PowScaleVector3Cpp(&m_vecMoveSpeed.x, 0.97F, CTimer::GetTimeStep());
        PowScaleVector3Cpp(&m_vecTurnSpeed.x, 0.97F, CTimer::GetTimeStep());
    }
    else if (m_nModelIndex != ModelIndices::MI_BUOY) {
        physicalFlags.bTouchingWater = false;
    }

    if (m_pObjectInfo->m_bCausesExplosion
        && objectFlags.bIsExploded
        && GetIsVisible()
        && (CGeneral::GetRandomNumber() % 32) == 10)
    {
        SetUsesCollision(false);
        SetIsVisible(false);
        physicalFlags.bExplosionProof = true;
        ResetMoveSpeed();
        physicalFlags.bApplyGravity = false;
        DeleteRwObject();
    }

    if (m_nModelIndex == ModelIndices::MI_RCBOMB) {
        // Como el original: fuerza = velocidad · (−0.05 · masa de giro) en −forward y pow(1 − |v|²·0.2, paso)
        const auto& fwd = m_matrix->GetForward();
        const CVector vecPoint{ -fwd.x, -fwd.y, -fwd.z };
        const float fForceMult = -0.05F * m_fTurnMass;
        CPhysical::ApplyTurnForce(CVector{ fForceMult * m_vecMoveSpeed.x, fForceMult * m_vecMoveSpeed.y, fForceMult * m_vecMoveSpeed.z }, vecPoint);
        const float fBase = 1.0F - ObjSqMag(m_vecMoveSpeed) * 0.2F;
        PowScaleVector3Cpp(&m_vecMoveSpeed.x, fBase, CTimer::GetTimeStep());
    }

    if (m_bIsBIGBuilding)
        SetIsInSafePosition(true);

    if (physicalFlags.bDisableMoveForce && m_fDoorStartAngle > -1000.0F) {
        auto fDiff = ObjectDoorAngleDiffCpp(m_matrix ? &m_matrix->GetForward().x : nullptr, m_placement.m_fHeading, m_fDoorStartAngle);
        if (std::fabs(fDiff) > ObjF(0x3F060A92u)) // 0x866F58, PI/6
            objectFlags.bIsDoorOpen = true;

        // Como el original: límites de las variables globales del EXE (0x8D0A2C..0x8D0A34)
        if (fDiff > fMaxDoorDiff)
            fDiff = fMaxDoorDiff;
        else if (fDiff < -fMaxDoorDiff)
            fDiff = -fMaxDoorDiff;

        if (fDiff > 0.0F && m_vecTurnSpeed.z < fDoorCutoffSpeed ||
            fDiff < 0.0F && -fDoorCutoffSpeed < m_vecTurnSpeed.z
        ) {
            m_vecTurnSpeed.z = CTimer::GetTimeStep() * fDoorSpeedMult * fDiff + m_vecTurnSpeed.z;
        }

        if (fDiff != 0.0F && objectFlags.bIsDoorMoving) {
            AudioEngine.ReportDoorMovement(this);
        }

        // Como el original: se comprueba m_bIsStuck (no m_bIsBIGBuilding)
        if (!GetIsStuck()
            && !GetIsStatic()
            && std::fabs(fDiff) < 0.01F
            && (objectFlags.bIsDoorMoving || std::fabs(m_vecTurnSpeed.z) < 0.01F)
        ) {
            SetIsStatic(true);
            ResetMoveSpeed();
            ResetTurnSpeed();
            ResetFrictionMoveSpeed();
            ResetFrictionTurnSpeed();

            if (objectFlags.bIsDoorMoving && objectFlags.bIsDoorOpen)
                LockDoor();
        }
    }
}

// 0x5A17B0
void CObject::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);
    SetPosn(destination);
    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
    CWorld::Add(this);
}

// 0x59FEE0
void CObject::SpecialEntityPreCollisionStuff(CPhysical* colPhysical, bool bIgnoreStuckCheck, bool& bCollisionDisabled, bool& bCollidedEntityCollisionIgnored, bool& bCollidedEntityUnableToMove, bool& bThisOrCollidedEntityStuck) {
    if (m_pEntityIgnoredCollision == colPhysical || colPhysical->m_pEntityIgnoredCollision == this) {
        bCollidedEntityCollisionIgnored = true;
        return;
    }

    if (m_pAttachedTo == colPhysical
        || colPhysical->m_pAttachedTo == this
        || m_pAttachedTo && m_pAttachedTo == colPhysical->m_pAttachedTo
        || physicalFlags.bDisableZ && !physicalFlags.bApplyGravity && !colPhysical->physicalFlags.bDisableZ
    ) {
        bCollisionDisabled = true;
    } else if (physicalFlags.bDisableZ) {
        if (bIgnoreStuckCheck)
            bCollisionDisabled = true;
        else if (GetIsStuck() || colPhysical->GetIsStuck())
            bThisOrCollidedEntityStuck = true;
    } else if (physicalFlags.bDisableMoveForce || physicalFlags.bInfiniteMass) {
        if (bIgnoreStuckCheck || GetIsStuck())
            bCollisionDisabled = true;
        else if (colPhysical->GetIsStuck()) {
            if (!colPhysical->GetHasHitWall())
                bThisOrCollidedEntityStuck = true;
            else
                bCollidedEntityUnableToMove = true;
        }
    } else {
        // Como el original: una farola caída solo ignora vehículos y peatones; con otras entidades se siguen las
        // comprobaciones generales. Con un vehículo lo guarda como entidad ignorada; con un peatón sale sin la
        // comprobación final.
        bool bHandled = false;
        if (objectFlags.bIsLampPost && (m_matrix->GetUp().z < 0.66F || GetIsStuck())
            && (colPhysical->GetIsTypeVehicle() || colPhysical->GetIsTypePed())
        ) {
            bCollidedEntityCollisionIgnored = true;
            if (!colPhysical->GetIsTypeVehicle())
                return;

            m_pEntityIgnoredCollision = colPhysical;
            bHandled = true;
        }

        if (bHandled) {
            // Nada más
        } else if (colPhysical->GetIsTypeVehicle()) {
            if (IsModelTempCollision()) {
                bCollisionDisabled = true;
            } else if (IsTemporary() || IsExploded() || !CEntity::GetIsStatic()) {
                if (colPhysical->AsVehicle()->IsConstructionVehicle()) {
                    if (GetIsStuck() || colPhysical->GetIsStuck())
                        bThisOrCollidedEntityStuck = true;
                } else if (!CanBeSmashed()) {
                    // Como el original: solo objetos cuya caja acaba por debajo de 1 en los tres ejes
                    const auto& box = CEntity::GetColModel()->GetBoundingBox();
                    if (box.m_vecMax.x < 1.0F && box.m_vecMax.y < 1.0F && box.m_vecMax.z < 1.0F) {
                        CMatrix tempMat;
                        const auto vecSize = box.m_vecMax - box.m_vecMin;
                        const auto vecTransformed = ObjMultiplyPoint(*m_matrix, vecSize);
                        if (vecTransformed.z < colPhysical->GetPosition().z) {
                            bCollidedEntityCollisionIgnored = true;
                            m_pEntityIgnoredCollision = colPhysical;
                        } else {
                            Invert(*colPhysical->m_matrix, tempMat);
                            if (ObjMultiplyPoint(tempMat, vecTransformed).z < 0.0F) {
                                bCollidedEntityCollisionIgnored = true;
                                m_pEntityIgnoredCollision = colPhysical;
                            }
                        }
                    }
                }
            }
        } else if (m_nModelIndex == MODEL_GRENADE
            && colPhysical->GetIsTypePed()
            && m_matrix->GetPosition().z < colPhysical->m_matrix->GetPosition().z
        ) {
            // Como el original: la granada por debajo del peatón no colisiona con él
            bCollisionDisabled = true;
        } else if (colPhysical->GetIsTypeObject()
            && colPhysical->AsObject()->m_pObjectInfo->m_fUprootLimit > 0.0F
            && !colPhysical->m_pAttachedTo
            && (!colPhysical->physicalFlags.bDisableCollisionForce || colPhysical->physicalFlags.bCollidable)
            && colPhysical->m_fMass * 10.0F > m_fMass
        ) {
            bCollidedEntityUnableToMove = true;
        }
    }

    if (!bCollidedEntityCollisionIgnored && (bIgnoreStuckCheck || GetIsStuck()))
        bThisOrCollidedEntityStuck = true;
}

// 0x5A02E0
// Como el original: magnitudes de 0x406DA0, (|v|²·paso)·paso, comparaciones en las que NaN devuelve 1, raíz
// por las constantes inversas del EXE (6.6666665 y 3.3333333) y ceil del CRT + _ftol2.
uint8 CObject::SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2) {
    const float ts = CTimer::GetTimeStep();
    if (physicalFlags.bDisableZ || m_pObjectInfo->m_nSpecialColResponseCase == COL_SPECIAL_RESPONSE_GRENADE) {
        const float radius = GetModelInfo()->GetColModel()->GetBoundRadius();
        const float fMove = ObjSqMag(m_vecMoveSpeed) * ts * ts;
        if (!(fMove >= radius * radius))
            return 1;

        return ObjCeilSteps(std::sqrt(fMove) / radius);
    }

    if (physicalFlags.bDisableMoveForce) {
        // Como el original: mayor de max.x/max.y frente al menor de min.x/min.y (se guarda sin negar) y fabs
        const auto& box = CEntity::GetColModel()->GetBoundingBox();
        float fLongest = box.m_vecMax.y < box.m_vecMax.x ? box.m_vecMax.x : box.m_vecMax.y;
        const float fMin = box.m_vecMin.y > box.m_vecMin.x ? box.m_vecMin.x : box.m_vecMin.y;
        if (-fMin > fLongest)
            fLongest = fMin;

        const float fMove = std::fabs(fLongest * m_vecTurnSpeed.z);
        if (!(fMove > 0.1F))
            return 1;

        return ObjCeilSteps(fMove * 10.0F);
    }

    if (physicalFlags.bInfiniteMass) {
        auto* cm = CEntity::GetColModel();
        const auto vecMin = ObjMultiply3x3(*m_matrix, cm->GetBoundingBox().m_vecMin);
        const auto vecSpeed = CPhysical::GetSpeed(vecMin);
        // Como el original: aquí la magnitud se suma z, y, x
        const float fMove = ((vecSpeed.z * vecSpeed.z + vecSpeed.y * vecSpeed.y) + vecSpeed.x * vecSpeed.x) * ts * ts;
        if (!(fMove >= 0.0225F))
            return 1;

        return ObjCeilSteps(std::sqrt(fMove) * ObjF(0x40D55555u)); // 0x866FBC
    }

    if (IsTemporary() && !objectFlags.bIsLiftable)
        return 1;

    const auto special = m_pObjectInfo->m_nSpecialColResponseCase;
    if (special == COL_SPECIAL_RESPONSE_LAMPOST) {
        // Como el original: primero los dos Multiply3x3 (arriba y abajo) y luego GetSpeed de arriba y de abajo
        auto* cm = GetModelInfo()->GetColModel();
        const auto vecTop    = ObjMultiply3x3(*m_matrix, CVector(0.0F, 0.0F, cm->GetBoundingBox().m_vecMax.z));
        const auto vecBottom = ObjMultiply3x3(*m_matrix, CVector(0.0F, 0.0F, cm->GetBoundingBox().m_vecMin.z));
        const auto vecSpeedTop    = CPhysical::GetSpeed(vecTop);
        const auto vecSpeedBottom = CPhysical::GetSpeed(vecBottom);
        const auto& vecUsed = ObjSqMag(vecSpeedBottom) < ObjSqMag(vecSpeedTop) ? vecSpeedTop : vecSpeedBottom;
        const float fMove = ObjSqMag(vecUsed) * ts * ts;
        if (!(fMove >= 0.09F))
            return 1;

        return ObjCeilSteps(std::sqrt(fMove) * ObjF(0x40555555u)); // 0x859F60
    }

    if (objectFlags.bIsLiftable || special == COL_SPECIAL_RESPONSE_SMALLBOX || special == COL_SPECIAL_RESPONSE_FENCEPART) {
        // Como el original: la arista usada es la MENOR de la caja por fObjectStepSizeMult (0x8D0A28)
        const auto& box = CEntity::GetColModel()->GetBoundingBox();
        const float sizeX = box.m_vecMax.x - box.m_vecMin.x;
        const float sizeY = box.m_vecMax.y - box.m_vecMin.y;
        const float sizeZ = box.m_vecMax.z - box.m_vecMin.z;
        float fEdge = sizeY < sizeX ? sizeY : sizeX;
        fEdge = sizeZ < fEdge ? sizeZ : fEdge;
        fEdge = fObjectStepSizeMult * fEdge;
        const float fMove = ObjSqMag(m_vecMoveSpeed) * ts * ts;
        if (!(fMove >= fEdge * fEdge))
            return 1;

        return ObjCeilSteps(std::sqrt(fMove) / fEdge);
    }

    const float fMove = ObjSqMag(m_vecMoveSpeed) * ts * ts;
    if (!(fMove >= 0.09F))
        return 1;

    return ObjCeilSteps(std::sqrt(fMove) * ObjF(0x40555555u)); // 0x859F60
}

// 0x59FD50
void CObject::PreRender() {
    if (objectFlags.bAffectedByColBrightness)
        GetLightingFromCollisionBelow();

    if (m_nBurnTime > CTimer::GetTimeInMS())
        DoBurnEffect();

    if (!m_pAttachedTo) {
        // Como el original: GetCurrentLighting en línea con 1/30 (0x858F10) y (día·k)·(1 − b) + (noche·k)·b
        const float balance = CCustomBuildingDNPipeline::m_fDNBalanceParam;
        const float k = ObjF(0x3D088889u);
        m_fContactSurfaceBrightness = (static_cast<float>(m_nColLighting.day) * k) * (1.0F - balance)
                                    + (static_cast<float>(m_nColLighting.night) * k) * balance;
    }

    if (GetRwObject() && RwObjectGetType(GetRwObject()) == rpCLUMP && objectFlags.bFadingIn)
    {
        auto iAlpha = CVisibilityPlugins::GetClumpAlpha(GetRpClump()) - 16;
        iAlpha = std::max(0, iAlpha);
        CVisibilityPlugins::SetClumpAlpha(GetRpClump(), iAlpha);
    }

    CEntity::PreRender();

    if (m_fScale != 1.0F || objectFlags.bIsScaled)
    {
        auto vecScale = CVector(m_fScale, m_fScale, m_fScale);
        CEntity::UpdateRwMatrix();
        RwMatrixScale(CEntity::GetModellingMatrix(), &vecScale, RwOpCombineType::rwCOMBINEPRECONCAT);
        CEntity::UpdateRwFrame();
        objectFlags.bIsScaled = true;
        if (objectFlags.bIsScaled)
            objectFlags.bIsScaled = false; //BUG? It's unsetting the flag straight after setting it
    }

    if (GetRwObject() && RwObjectGetType(GetRwObject()) == rpCLUMP)
        CEntity::UpdateRpHAnim();
}

// 0x59F180
void CObject::Render() {
    if (objectFlags.bDoNotRender)
        return;

    const auto iRefModel = m_nRefModelIndex;
    if (iRefModel != -1 && IsTemporary() && objectFlags.bChangesVehColor) {
        auto* vehModelInfo = CModelInfo::GetModelInfo(iRefModel)->AsVehicleModelInfoPtr();
        CVehicleModelInfo::ms_pRemapTexture = m_pRemapTexture;
        vehModelInfo->SetVehicleColour(m_nCarColor[0], m_nCarColor[1], m_nCarColor[2], m_nCarColor[3]);
    }

    CEntity::Render();
}

// 0x554FA0
bool CObject::SetupLighting() {
    if (physicalFlags.bRenderScorched) {
        WorldReplaceNormalLightsWithScorched(Scene.m_pRpWorld, 0.18F);
        return true;
    }

    if (m_bLightObject) {
        ActivateDirectional();
        return CRenderer::SetupLightingForEntity(this);
    }

    return false;
}

// 0x553E10
void CObject::RemoveLighting(bool bRemove) {
    if (!bRemove)
        return;

    if (!physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();

    SetAmbientColours();
    DeActivateDirectional();
}

// 0x5D2870 - Deserializes object from save storage buffer
bool CObject::Load() {
    auto size = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
    auto data = CGenericGameStorage::LoadDataFromWorkBuffer<CObjectSaveStructure>();
    assert(size == sizeof(data)); // Check structure size.
    data.Extract(this);
    return true;
}

// 0x5D2830 - Serializes object to save storage buffer
bool CObject::Save() {
    CObjectSaveStructure data;
    data.Construct(this);
    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(CObjectSaveStructure));
    CGenericGameStorage::SaveDataToWorkBuffer(data);
    return true;
}

// 0x44A4D0
void CObject::ProcessGarageDoorBehaviour() {
    if (m_nGarageDoorGarageIndex < 0)
        m_nGarageDoorGarageIndex = static_cast<int8>(CGarages::FindGarageForObject(this));

    if (m_nGarageDoorGarageIndex < 0)
        return;

    const auto& vecDummyPos = m_pDummyObject->GetPosition();
    auto* mi = GetModelInfo();
    const auto fHeight = mi->GetColModel()->GetBoundingBox().GetHeight() - 0.1f;
    const auto& garage = CGarages::GetGarage(m_nGarageDoorGarageIndex);
    if (garage.m_bDoorOpensUp) {
        m_matrix->GetPosition().z = vecDummyPos.z + fHeight * garage.m_fDoorPosition * 0.48F;
        float fDoorPos = garage.m_fDoorPosition;
        if (garage.m_bDoorGoesIn)
            fDoorPos = -fDoorPos;

        CGarage::BuildRotatedDoorMatrix(this, fDoorPos);
    } else {
        if (garage.m_nType == eGarageType::HANGAR_AT400)
            m_matrix->GetPosition().z = vecDummyPos.z - fHeight * garage.m_fDoorPosition;
        else if (garage.m_nType == eGarageType::HANGAR_ABANDONED_AIRPORT)
            m_matrix->GetPosition().x = vecDummyPos.x - garage.m_fDoorPosition * m_matrix->GetRight().x * 14.0F;
        else
            m_matrix->GetPosition().z = fHeight * garage.m_fDoorPosition * ObjF(0x3F68BA2Eu) + vecDummyPos.z; // Como el original: · 0x8599A0 (no /1.1)
    }

    SetUsesCollision(garage.m_bDoorClosed);
    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
}

// 0x59F120
bool CObject::CanBeDeleted() const {
    switch (m_nObjectType) {
    case eObjectType::OBJECT_MISSION:
    case eObjectType::OBJECT_TYPE_CUTSCENE:
    case eObjectType::OBJECT_TYPE_DECORATION:
    case eObjectType::OBJECT_MISSION2:
        return false;

    default:
        return true;
    }
}

// 0x59F160
void CObject::SetRelatedDummy(CDummyObject* relatedDummy) {
    assert(relatedDummy);
    m_pDummyObject = relatedDummy;
    m_pDummyObject->RegisterReference(reinterpret_cast<CEntity**>(&m_pDummyObject));
}

// 0x59F2D0
bool CObject::TryToExplode() {
    if (!m_pObjectInfo->m_bCausesExplosion)
        return false;

    if (objectFlags.bIsExploded)
        return false;

    objectFlags.bIsExploded = true;
    Explode();
    return true;
}

// 0x59F300
void CObject::SetObjectTargettable(bool targetable) {
    objectFlags.bIsTargetable = targetable;
}

// 0x59F320
bool CObject::CanBeTargetted() const {
    return objectFlags.bIsTargetable;
}

// 0x59F330
void CObject::RefModelInfo(int32 modelIndex) {
    m_nRefModelIndex = modelIndex;
    CModelInfo::GetModelInfo(modelIndex)->AddRef();
}

// 0x59F350
// Como el original: el índice llega como dword y se compara entero con -1
void CObject::SetRemapTexture(RwTexture* remapTexture, int32 txdIndex) {
    m_pRemapTexture = remapTexture;
    m_wRemapTxd = static_cast<int16>(txdIndex);
    if (txdIndex != -1)
        CTxdStore::AddRef(txdIndex);
}

// 0x59F380
float CObject::GetRopeHeight() {
    const auto ropeIndex = CRopes::FindRope(reinterpret_cast<uint32>(this));
    return CRopes::GetRope(ropeIndex).m_fSegmentLength;
}

// 0x59F3A0
void CObject::SetRopeHeight(float height) {
    const auto ropeIndex = CRopes::FindRope(reinterpret_cast<uint32>(this));
    CRopes::GetRope(ropeIndex).m_fSegmentLength = height;
}

// 0x59F3C0
// Como el original: devuelve el objeto recogido (+0x318, m_pRopeAttachObject), no el gancho
CEntity* CObject::GetObjectCarriedWithRope() {
    const auto ropeIndex = CRopes::FindRope(reinterpret_cast<uint32>(this));
    return CRopes::GetRope(ropeIndex).m_pRopeAttachObject;
}

// 0x59F3E0
void CObject::ReleaseObjectCarriedWithRope() {
    const auto ropeIndex = CRopes::FindRope(reinterpret_cast<uint32>(this));
    CRopes::GetRope(ropeIndex).ReleasePickedUpObject();
}

// 0x59F400
void CObject::AddToControlCodeList() {
   m_pControlCodeList = CWorld::ms_listObjectsWithControlCode.AddItem(this);
}

// 0x59F450
void CObject::RemoveFromControlCodeList() {
    if (!m_pControlCodeList)
        return;

    CWorld::ms_listObjectsWithControlCode.DeleteNode(m_pControlCodeList);
    m_pControlCodeList = nullptr;
}

// 0x59F4B0
void CObject::ResetDoorAngle() {
    if (!physicalFlags.bDisableMoveForce || m_fDoorStartAngle <= -1000.0F)
        return;

    CPlaceable::SetHeading(m_fDoorStartAngle);
    SetIsStatic(true);
    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetFrictionMoveSpeed();
    ResetFrictionTurnSpeed();
    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
}

// 0x59F5C0
void CObject::LockDoor() {
    objectFlags.bIsDoorOpen = false;
    physicalFlags.bCollidable = true;
    physicalFlags.bDisableCollisionForce = true;
    ResetDoorAngle();
}

// 0x59F840
// Aplica los valores iniciales comunes de un objeto físico y, si hay un modelo
// asignado, obtiene de este propiedades de colisión y representación.
void CObject::Init() {
    SetTypeObject();
    m_pObjectInfo = &CObjectData::GetDefault();
    m_nColDamageEffect = COL_DAMAGE_EFFECT_NONE;
    m_nSpecialColResponseCase = COL_SPECIAL_RESPONSE_NONE;
    m_nObjectType = eObjectType::OBJECT_GAME;
    SetIsStatic(true);

    // Como el original: se conservan los bits 18 y 26..31 y bCanBeAttachedToMagnet queda siempre activo
    m_nObjectFlags = (m_nObjectFlags & 0xFC040000) | 0x40000;
    if (m_nModelIndex == 0xFFFF) {
        objectFlags.bHasNoModel = false;
    } else {
        CObjectData::SetObjectData(static_cast<int16>(m_nModelIndex), *this);
        auto* mi = GetModelInfo();
        auto* cm = mi->GetColModel();
        if (cm->m_bHasCollisionVolumes) {
            CColStore::AddRef(cm->m_nColSlot);
            objectFlags.bHasNoModel = true;
        }

        // Como el original: el centro de masas de los modelos que se balancean se ajusta haya o no volúmenes
        auto* ami = mi->AsAtomicModelInfoPtr();
        if (ami && ami->SwaysInWind() && !physicalFlags.bDisableCollisionForce) {
            auto& bbox = cm->GetBoundingBox();
            m_vecCentreOfMass.z = (bbox.m_vecMax.z - bbox.m_vecMin.z) * 0.2F + bbox.m_vecMin.z;
        }
    }

    if (physicalFlags.bDisableMoveForce) {
        if (auto* cd = CEntity::GetColModel()->m_pColData) {
            cd->m_nNumSpheres = 0;
        }
    }

    m_fHealth = 1000.0F;
    m_fDoorStartAngle = -1001.0F;
    m_nRemovalTime = 0;
    m_nBonusValue = 0;
    m_wCostValue = 0;
    // Como el original: solo se ponen a cero los dos primeros colores
    m_nCarColor[1] = 0;
    m_nCarColor[0] = 0;

    m_nRefModelIndex = -1;
    m_nGarageDoorGarageIndex = -1;
    m_nLastWeaponDamage = -1;
    m_pFire = nullptr;

    if (m_nModelIndex == ModelIndices::MI_BUOY)
        physicalFlags.bTouchingWater = true;

    auto mi = GetModelInfo();
    if (m_nModelIndex != 0xFFFF && mi->GetModelType() == ModelInfoType::MODEL_INFO_WEAPON)
        m_bLightObject = true;

    if (m_nModelIndex == ModelIndices::MI_MLAMPPOST
        || m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI
        || m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_VEGAS
        || m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL
        || m_nModelIndex == ModelIndices::MI_SINGLESTREETLIGHTS1
        || m_nModelIndex == ModelIndices::MI_SINGLESTREETLIGHTS2
        || m_nModelIndex == ModelIndices::MI_SINGLESTREETLIGHTS3
        || m_nModelIndex == ModelIndices::MI_DOUBLESTREETLIGHTS
        || m_nModelIndex != 0xFFFF && mi->AsAtomicModelInfoPtr() && mi->SwaysInWind()
    ) {
        objectFlags.bIsLampPost = true;
    } else {
        objectFlags.bIsLampPost = false;
    }

    objectFlags.bIsTargetable = false;
    physicalFlags.bAttachedToEntity = false;

    SetAreaCode(AREA_CODE_13);
    m_wRemapTxd = -1;
    m_pRemapTexture = nullptr;
    m_pControlCodeList = nullptr;

    if (m_nModelIndex == ModelIndices::MI_SAMSITE
        || m_nModelIndex == ModelIndices::MI_SAMSITE2
        || m_nModelIndex == ModelIndices::MI_TRAINCROSSING
        || m_nModelIndex == ModelIndices::MI_TRAINCROSSING1
        || m_nModelIndex == ModelIndices::MI_MAGNOCRANE
        || m_nModelIndex == ModelIndices::MI_CRANETROLLEY
        || m_nModelIndex == ModelIndices::MI_QUARRYCRANE_ARM
        || CGarages::IsModelIndexADoor(static_cast<int16>(m_nModelIndex))
    ) {
        AddToControlCodeList();
    }

    m_nBurnTime = 0;
    m_fScale = 1.0F;

    m_nColLighting.day = 0x8;
    m_nColLighting.night = 0x4;
    m_nStreamedScriptBrainToLoad = -1;
}

// 0x59FB50
void CObject::DoBurnEffect() const {
    const auto& box = GetModelInfo()->GetColModel()->GetBoundingBox();
    const auto& vecSize = box.GetSize();
    // Como el original: producto por 0.05 (0x858C28) y _ftol2
    const auto nUsedSize = ObjFtol(vecSize.x * vecSize.y * vecSize.z * m_fBurnDamage * 0.05F);
    if (nUsedSize <= 0)
        return;

    for (auto i = 0; i < nUsedSize; ++i) {
        const auto fRandX = CGeneral::GetRandomNumberInRange(box.m_vecMin.x, box.m_vecMax.x);
        const auto fRandY = CGeneral::GetRandomNumberInRange(box.m_vecMin.y, box.m_vecMax.y);
        const auto fRandZ = CGeneral::GetRandomNumberInRange(box.m_vecMin.z, box.m_vecMax.z);
        auto vecParticlePos = ObjMultiplyPoint(*m_matrix, CVector(fRandX, fRandY, fRandZ));

        g_fx.m_SmokeII3expand->AddParticle(vecParticlePos, { 0.0F, 0.0F, 0.02F }, 0.0F, FxPrtMult_c(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F, 0.4F));
    }
}

// 0x59FD00
void CObject::GetLightingFromCollisionBelow() {
    CColPoint colPoint{};
    CEntity* entity;
    if (CWorld::ProcessVerticalLine(GetPosition(), -1000.0F, colPoint, entity, true, false, false, false, true)) {
        m_nColLighting = colPoint.m_nLightingB;
    }
}

// 0x5A07D0
void CObject::ProcessSamSiteBehaviour() {
    if (objectFlags.bIsBroken)
        return;

    // Como el original: x < −1000 usa el indicador del portaaviones; si no (también NaN), el del Área 51
    const bool bDisabled = GetPosition().x < -1000.0F ? bAircraftCarrierSamSiteDisabled : bArea51SamSiteDisabled;
    if (bDisabled) {
        return;
    }

    const float fHeading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y);

    // Como el original: FindPlayerVehicle se vuelve a llamar en cada comprobación
    CEntity* targetEntity = nullptr;
    if (FindPlayerVehicle()
        && FindPlayerVehicle()->GetVehicleAppearance() != eVehicleAppearance::VEHICLE_APPEARANCE_BIKE
        && FindPlayerVehicle()->GetVehicleAppearance() != eVehicleAppearance::VEHICLE_APPEARANCE_AUTOMOBILE
    ) {
        targetEntity = FindPlayerVehicle();
    } else if (FindPlayerPed()->GetIntelligence()->GetTaskJetPack()) {
        targetEntity = FindPlayerPed();
    }

    if (targetEntity) {
        const auto& vecPos = GetPosition();
        const auto& vecTargetPos = targetEntity->GetPosition();
        // Como el original: (z − 2) > z del objetivo y y > 2100 descartan el objetivo (NaN no)
        if (!(vecPos.z - 2.0F > vecTargetPos.z) && !(vecTargetPos.y > 2100.0F)) {
            const float dirX = vecTargetPos.x - vecPos.x;
            const float dirY = vecTargetPos.y - vecPos.y;

            // Como el original: la diferencia se calcula con el ángulo completo de ST0 (fst del ángulo y fsub del rumbo)
            float fAngle, fAngleDiff;
            ObjectSamSiteAtanCpp(&CGeneral::GetATanOfXY, dirX, dirY, fHeading, &fAngle, &fAngleDiff);
            while (fAngleDiff > OBJ_PI)
                fAngleDiff -= OBJ_TWO_PI;
            while (fAngleDiff < OBJ_MINUS_PI)
                fAngleDiff += OBJ_TWO_PI;

            // Como el original: paso · 0.05 (0x858C28)
            const float fTimeStep = CTimer::GetTimeStep() * 0.05F;
            float fNewAngle;
            if (!(std::fabs(fAngleDiff) > fTimeStep))
                fNewAngle = fAngle;
            else if (fAngleDiff < 0.0F)
                fNewAngle = fHeading - fTimeStep;
            else
                fNewAngle = fTimeStep + fHeading;

            m_matrix->SetRotateZOnly(fNewAngle - OBJ_HALF_PI);
            CEntity::UpdateRwMatrix();
            CEntity::UpdateRwFrame();

            const float shootX = GetPosition().x - targetEntity->GetPosition().x;
            const float shootY = GetPosition().y - targetEntity->GetPosition().y;
            if (!(std::sqrt(shootY * shootY + shootX * shootX) < 120.0F))
                return;

            if (!(std::fabs(fAngleDiff) < 0.1F) || CTimer::GetTimeInMS() / 4000 == CTimer::GetPreviousTimeInMS() / 4000)
                return;

            auto vecRocketDir = m_matrix->GetForward() + m_matrix->GetUp();
            const auto vecSrcPos = ObjMultiplyPoint(*m_matrix, CVector(0.0F, 2.0F, 4.0F));
            CProjectileInfo::AddProjectile(this, eWeaponType::WEAPON_ROCKET_HS, vecSrcPos, 0.0F, &vecRocketDir, targetEntity);
            return;
        }
    }

    // Como el original: (paso · 0.005 + rumbo) − PI/2 directamente sobre la matriz
    m_matrix->SetRotateZOnly(CTimer::GetTimeStep() * 0.005F + fHeading - OBJ_HALF_PI);
    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
}

// 0x5A0B50
void CObject::ProcessTrainCrossingBehaviour() {
    if (!(static_cast<uint8>(CTimer::GetFrameCounter() + m_nRandomSeedUpperByte) & 0x10)) {
        const auto& vecPos = GetPosition();
        const auto bWasEnabled = objectFlags.bTrainCrossEnabled;
        auto* train = CTrain::FindNearestTrain(vecPos, true);
        objectFlags.bTrainCrossEnabled = false;
        if (train) {
            auto vecDist = train->GetPosition() - vecPos;
            if (vecDist.Magnitude2D() < 120.0F)
                objectFlags.bTrainCrossEnabled = true;
        }

        if (m_nModelIndex == ModelIndices::MI_TRAINCROSSING1 && objectFlags.bTrainCrossEnabled != bWasEnabled) {
            const auto& dummyPos = m_pDummyObject->GetPosition();
            ThePaths.SetLinksBridgeLights(dummyPos.x - 12.0F, dummyPos.x + 12.0F, dummyPos.y - 12.0F, dummyPos.y + 12.0F, !bWasEnabled);
        }
    }

    if (m_nModelIndex == ModelIndices::MI_TRAINCROSSING1)
        return;

    // Como el original: acos del CRT con ST0 completo ± paso · 0.005; límites 0 y 0x866FC8 (1.3508849)
    const float fTimeStep = CTimer::GetTimeStep() * 0.005F;
    const float fMaxAngle = ObjF(0x3FACE9CCu);
    float fAngle;
    if (objectFlags.bTrainCrossEnabled) {
        fAngle = ObjectTrainCrossingAngleCpp(m_matrix->GetUp().z, fTimeStep, true);
        if (!(fAngle > 0.0F))
            fAngle = 0.0F;
    } else {
        fAngle = ObjectTrainCrossingAngleCpp(m_matrix->GetUp().z, fTimeStep, false);
        if (!(fAngle < fMaxAngle))
            fAngle = fMaxAngle;
    }
    SetMatrixForTrainCrossing(m_matrix, fAngle);

    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
}

// 0x5A0D90
void CObject::ObjectDamage(float damage, const CVector* fxOrigin, const CVector* fxDirection, CEntity* damager, eWeaponType weaponType) {
    // Como el original: secuencia en línea de destrucción (0x5A0F7B, 0x5A1003, 0x5A16C4): quita colisión y
    // visibilidad, saca de la lista de móviles si no es estático, escribe el bit estático directamente (sin
    // SetIsStatic), activa bExplosionProof, anula velocidad y giro y borra el objeto RW.
    const auto SmashInline = [this](bool bBroken) {
        SetUsesCollision(false);
        SetIsVisible(false);
        if (!CEntity::GetIsStatic())
            CPhysical::RemoveFromMovingList();

        m_bIsStatic = true;
        physicalFlags.bExplosionProof = true;
        ResetMoveSpeed();
        ResetTurnSpeed();
        if (bBroken)
            objectFlags.bIsBroken = true;
        DeleteRwObject();
    };

    if (!GetUsesCollision())
        return;

    if (weaponType == eWeaponType::WEAPON_UNIDENTIFIED && damager && damager->GetIsTypeVehicle())
        weaponType = eWeaponType::WEAPON_RUNOVERBYCAR;

    if (!CPhysical::CanPhysicalBeDamaged(weaponType, nullptr))
        return;

    m_fHealth -= damage * m_pObjectInfo->m_fColDamageMultiplier;
    // Como el original: solo se recorta si es menor que 0 (NaN y −0 se conservan)
    if (m_fHealth < 0.0F)
        m_fHealth = 0.0F;

    if (!m_nColDamageEffect || physicalFlags.bInvulnerable && damager != FindPlayerPed() && damager != FindPlayerVehicle())
        return;

    // Big Smoke crack palace wall break checks
    if (m_nModelIndex == ModelIndices::MI_IMY_SHASH_WALL) {
        if (!damager)
            return;

        if (damager->GetIsTypePed()) {
            auto* ped = damager->AsPed();
            if (!ped->bInVehicle || !ped->m_pVehicle || ped->m_pVehicle->m_nModelIndex != MODEL_SWATVAN)
                return;
        } else if (damager->GetIsTypeVehicle()) {
            if (damager->m_nModelIndex != MODEL_SWATVAN)
                return;
        } else
            return;
    }

    if (damager && damager->m_nModelIndex == MODEL_FORKLIFT)
        return;

    m_nLastWeaponDamage = weaponType;
    bool bWasDestroyed = false;

    if (damage * m_pObjectInfo->m_fColDamageMultiplier > 150.0F || m_fHealth == 0.0F) {
        switch (m_nColDamageEffect) {
        case COL_DAMAGE_EFFECT_CHANGE_MODEL:
            if (!m_bRenderDamaged) {
                bWasDestroyed = true;
                DeleteRwObject();
            }
            m_bRenderDamaged = true;
            break;

        case COL_DAMAGE_EFFECT_SMASH_COMPLETELY:
            SmashInline(false);
            bWasDestroyed = true;
            break;

        case COL_DAMAGE_EFFECT_CHANGE_THEN_SMASH:
            if (m_bRenderDamaged) {
                SmashInline(false);
                bWasDestroyed = true;
            } else {
                DeleteRwObject();
                m_bRenderDamaged = true;
            }
            break;

        case COL_DAMAGE_EFFECT_BREAKABLE:
        case COL_DAMAGE_EFFECT_BREAKABLE_REMOVED: {
            // Como el original: se pasa una copia local de la velocidad de rotura
            CVector vecBreakVelocity = m_pObjectInfo->m_vecBreakVelocity;
            const auto bJustFaces = damage * m_pObjectInfo->m_fColDamageMultiplier > m_pObjectInfo->m_fSmashMultiplier * 150.0F;
            g_breakMan.Add(this, &vecBreakVelocity, m_pObjectInfo->m_fBreakVelocityRand, bJustFaces);
            SmashInline(true);
            bWasDestroyed = true;
            break;
        }
        }

        if (!GetUsesCollision() && !GetIsVisible()) {
            m_fHealth = 0.0F;
        }
    }

    if (bWasDestroyed) {
        const bool bExploded = TryToExplode();
        AudioEngine.ReportObjectDestruction(this);
        if (bExploded)
            return;
    }

    // Como el original: destruido solo reproduce PLAY_ON_DESTROYED; sin destruir, PLAY_ON_HIT con daño > 30;
    // PLAY_ON_HIT_DESTROYED siempre.
    switch (m_pObjectInfo->m_nFxType) {
    case eObjectFxType::PLAY_ON_HIT_DESTROYED:
        break;
    case eObjectFxType::PLAY_ON_DESTROYED:
        if (bWasDestroyed)
            break;
        return;
    case eObjectFxType::PLAY_ON_HIT:
        if (!bWasDestroyed && damage > 30.0F)
            break;
        return;
    default:
        return;
    }

    if (m_pObjectInfo->m_vFxOffset.x < -500.0F) {
        if (!fxOrigin)
            return;

        RwMatrix particleMat;
        g_fx.CreateMatFromVec(&particleMat, fxOrigin, fxDirection);
        auto* fxSystem = g_fxMan.CreateFxSystem(m_pObjectInfo->m_pFxSystemBP, particleMat, nullptr, false);
        if (fxSystem)
            fxSystem->PlayAndKill();

        return;
    }

    // Como el original: Multiply3x3 (0x59C790) del desplazamiento y la posición sumada una sola vez
    auto vecPoint = ObjMultiply3x3(*m_matrix, m_pObjectInfo->m_vFxOffset);
    vecPoint += GetPosition();
    auto* fxSystem = g_fxMan.CreateFxSystem(m_pObjectInfo->m_pFxSystemBP, vecPoint, nullptr, false);
    if (fxSystem)
        fxSystem->PlayAndKill();
}

// 0x5A1340
void CObject::Explode() {
    CVector vecPos = GetPosition();
    vecPos.z += 0.5F;
    auto* player = FindPlayerPed();
    CExplosion::AddExplosion(this, player, eExplosionType::EXPLOSION_OBJECT, vecPos, 100, true, -1.0F, false);
    if (m_nColDamageEffect == COL_DAMAGE_EFFECT_BREAKABLE || m_nColDamageEffect == COL_DAMAGE_EFFECT_BREAKABLE_REMOVED) {
        vecPos.z -= 1.0F;
        auto vecDir = CVector(0.0F, 0.0F, 1.0F);
        ObjectDamage(10'000.0F, &vecPos, &vecDir, this, eWeaponType::WEAPON_EXPLOSION);
    } else if (!physicalFlags.bDisableCollisionForce) {
        // Como el original: primero z += 0.5 y luego x, y += ((rand & 0xFF) − 128) · 0.0002 (0x85A7D0)
        m_vecMoveSpeed.z += 0.5F;
        m_vecMoveSpeed.x += static_cast<float>(static_cast<int32>(CGeneral::GetRandomNumber() & 0xFF) - 128) * ObjF(0x3951B717u);
        m_vecMoveSpeed.y += static_cast<float>(static_cast<int32>(CGeneral::GetRandomNumber() & 0xFF) - 128) * ObjF(0x3951B717u);

        if (GetIsStatic()) {
            SetIsStatic(false);
            CPhysical::AddToMovingList();
        }
    }

    if (m_pObjectInfo->m_nFxType == eObjectFxType::PLAY_ON_DESTROYED) {
        // Como el original: Multiply3x3 (0x59C790) del desplazamiento y la posición sumada una sola vez
        auto vecPoint = ObjMultiply3x3(*m_matrix, m_pObjectInfo->m_vFxOffset);
        vecPoint += GetPosition();
        auto* fxSystem = g_fxMan.CreateFxSystem(m_pObjectInfo->m_pFxSystemBP, vecPoint, nullptr, false);
        if (fxSystem)
            fxSystem->PlayAndKill();
    }
}

// 0x5A1580
void CObject::ObjectFireDamage(float damage, CEntity* damager) {
    // Como el original: secuencia en línea de destrucción (0x5A0F7B, 0x5A1003, 0x5A16C4): quita colisión y
    // visibilidad, saca de la lista de móviles si no es estático, escribe el bit estático directamente (sin
    // SetIsStatic), activa bExplosionProof, anula velocidad y giro y borra el objeto RW.
    const auto SmashInline = [this](bool bBroken) {
        SetUsesCollision(false);
        SetIsVisible(false);
        if (!CEntity::GetIsStatic())
            CPhysical::RemoveFromMovingList();

        m_bIsStatic = true;
        physicalFlags.bExplosionProof = true;
        ResetMoveSpeed();
        ResetTurnSpeed();
        if (bBroken)
            objectFlags.bIsBroken = true;
        DeleteRwObject();
    };

    if (m_nModelIndex != ModelIndices::MI_GRASSPLANT && m_nModelIndex != ModelIndices::MI_GRASSHOUSE)
        return;

    m_fHealth -= damage;
    // Como el original: solo se recorta si es menor que 0 (NaN y −0 se conservan)
    if (m_fHealth < 0.0F)
        m_fHealth = 0.0F;

    if (!m_nColDamageEffect || physicalFlags.bInvulnerable && damager != FindPlayerPed() && damager != FindPlayerVehicle())
        return;

    if (m_nModelIndex == ModelIndices::MI_GRASSPLANT)
    {
        // Como el original: 1 − salud · 0.0005 (0x859978)
        m_fBurnDamage = 1.0F - m_fHealth * 0.0005F;
        m_nBurnTime = CTimer::GetTimeInMS() + 3000;
    }

    if (m_fHealth != 0.0F)
        return;

    m_nBurnTime = 0;

    if (m_nColDamageEffect == COL_DAMAGE_EFFECT_BREAKABLE
     || m_nColDamageEffect == COL_DAMAGE_EFFECT_BREAKABLE_REMOVED
    ) {
        if (!objectFlags.bIsBroken)
            AudioEngine.ReportObjectDestruction(this);

        CVector vecBreakVelocity = m_pObjectInfo->m_vecBreakVelocity;
        g_breakMan.Add(this, &vecBreakVelocity, m_pObjectInfo->m_fBreakVelocityRand, true);
        SmashInline(true);
    } else if (m_nColDamageEffect == COL_DAMAGE_EFFECT_CHANGE_MODEL && !m_bRenderDamaged) {
        m_bRenderDamaged = true;
        DeleteRwObject();
        CVector vecPos = GetPosition();
        CShadows::AddPermanentShadow(eShadowType::SHADOW_DEFAULT, gpShadowHeliTex, &vecPos, 3.0F, 0.0F, 0.0F, -3.0F, 200, 0, 0, 0, 10.0F, 30'000, 1.0F);
    }
}

// 0x5A1840
void CObject::TryToFreeUpTempObjects(int32 numObjects) {
    // Como el original: recorre el pool del último slot al primero y para en cuanto no quedan objetos por liberar
    auto* pool = GetObjectPool();
    for (auto i = static_cast<int32>(pool->GetSize()); i > 0;) {
        --i;
        if (numObjects <= 0)
            return;

        if (pool->IsFreeSlotAtIndex(i))
            continue;

        auto* obj = pool->GetAt(i);
        if (obj->IsTemporary() && !obj->IsVisible()) {
            CWorld::Remove(obj);
            delete obj;
            --numObjects;
        }
    }
}

// 0x5A18B0
void CObject::DeleteAllTempObjects() {
    for (auto& obj : GetObjectPool()->GetAllValid()) {
        if (obj.IsTemporary()) {
            CWorld::Remove(&obj);
            delete &obj;
        }
    }
}

// 0x5A1910
void CObject::DeleteAllMissionObjects() {
    for (auto& obj : GetObjectPool()->GetAllValid())  {
        if (obj.IsMissionObject()) {
            CWorld::Remove(&obj);
            delete &obj;
        }
    }
}

// 0x5A1980
void CObject::DeleteAllTempObjectsInArea(CVector point, float radius) {
    for (auto& obj : GetObjectPool()->GetAllValid()) {
        if (!obj.IsTemporary())
            continue;

        // Como el original: (punto − posición) sumando z, y, x
        const auto& pos = obj.GetPosition();
        const float dz = point.z - pos.z;
        const float dy = point.y - pos.y;
        const float dx = point.x - pos.x;
        if (radius * radius > (dz * dz + dy * dy) + dx * dx) {
            CWorld::Remove(&obj);
            delete &obj;
        }
    }
}

bool CObject::IsCraneMovingPart() const {
    return notsa::contains<eModelID>({
        ModelIndices::MI_CRANE_MAGNET,
        ModelIndices::MI_CRANE_HARNESS,
        ModelIndices::MI_MINI_MAGNET,
        ModelIndices::MI_WRECKING_BALL,
    }, GetModelId());
}

// 0x5A1AB0
// Como el original: el objeto recogido va a +0x318 (m_pRopeAttachObject, con su referencia) y el gancho (+0x314,
// m_pAttachedEntity) se coloca en attachTo · (0, 0, altura de recogida) y deja de usar colisión.
void CObject::GrabObjectToCarryWithRope(CPhysical* attachTo) {
    const auto iRopeInd = CRopes::FindRope(reinterpret_cast<uint32>(this));
    auto& rope = CRopes::GetRope(iRopeInd);
    rope.ReleasePickedUpObject();
    rope.m_pRopeAttachObject = attachTo;
    attachTo->RegisterReference(&rope.m_pRopeAttachObject);

    const float fPickupHeight = CRopes::FindPickupHeight(attachTo);
    const auto vecRopePoint = ObjMultiplyPoint(*rope.m_pRopeAttachObject->m_matrix, CVector(0.0F, 0.0F, fPickupHeight));

    rope.m_pAttachedEntity->SetPosn(vecRopePoint);
    rope.m_pAttachedEntity->SetUsesCollision(false);
}

// 0x5A1B60
bool CObject::CanBeUsedToTakeCoverBehind() {
    if (m_nObjectType == eObjectType::OBJECT_MISSION)
        return false;

    if (m_nModelIndex == ModelIndices::MI_FIRE_HYDRANT)
        return true;

    auto* mi = GetModelInfo();
    if (mi->AsAtomicModelInfoPtr() && mi->AsAtomicModelInfoPtr()->IsBreakableStatuePart())
        return false;

    // Como el original: altura < 1.25 y > 0.75 y up.z > 0.9; NaN no pasa
    const auto fHeight = mi->GetColModel()->GetBoundingBox().GetHeight();
    if (!(fHeight < 1.25F) || !(fHeight > 0.75F) || !(m_matrix->GetUp().z > 0.9F))
        return false;

    return true;
}

// 0x5A1F60
CObject* CObject::Create(int32 modelIndex, bool bUnused) {
    GetObjectPool()->SetDealWithNoMemory(true);
    auto* obj = new CObject(modelIndex, false); //BUG? most likely the unused parameter was supposed to be passed to the constructor
    GetObjectPool()->SetDealWithNoMemory(false);

    if (obj)
        return obj;

    TryToFreeUpTempObjects(5);
    g_waterCreatureMan.TryToFreeUpWaterCreatures(5);

    return new CObject(modelIndex, false);
}

// 0x5A2070
CObject* CObject::Create(CDummyObject* dummyObject) {
    GetObjectPool()->SetDealWithNoMemory(true);
    auto* obj = new CObject(dummyObject);
    GetObjectPool()->SetDealWithNoMemory(false);

    if (obj)
        return obj;

    TryToFreeUpTempObjects(5);
    g_waterCreatureMan.TryToFreeUpWaterCreatures(5);

    return new CObject(dummyObject);
}

// 0x59F200
// Como el original: fcos/fsin completos en el x87; forward = (0·sen + x·cos, 0·sen + y·cos, sen + z·cos) (el 0·sen
// fija el signo de los ceros) y se escribe up antes que forward.
void CObject::SetMatrixForTrainCrossing(CMatrix* matrix, float fAngle) {
    const auto vecAxis = CrossProduct(CVector(0.0F, 0.0F, 1.0F), matrix->GetRight());
    CVector vecForward;
    ObjectTrainCrossingForwardCpp(&vecAxis.x, fAngle, &vecForward.x);

    matrix->GetUp() = CrossProduct(matrix->GetRight(), vecForward);
    matrix->GetForward() = vecForward;
}

// 0x5A29A0
void CObject::ProcessControlLogic() {
    if (m_nModelIndex == ModelIndices::MI_SAMSITE || m_nModelIndex == ModelIndices::MI_SAMSITE2)
        ProcessSamSiteBehaviour();

    if (m_nModelIndex == ModelIndices::MI_TRAINCROSSING || m_nModelIndex == ModelIndices::MI_TRAINCROSSING1)
        ProcessTrainCrossingBehaviour();

    if (m_nModelIndex == ModelIndices::MI_MAGNOCRANE
        || m_nModelIndex == ModelIndices::MI_CRANETROLLEY
        || m_nModelIndex == ModelIndices::MI_QUARRYCRANE_ARM
    ) {
        const auto iRopeInd = CRopes::FindRope(reinterpret_cast<uint32>(this));
        int32 nSegments = 0;
        auto fRopeLengthChange = 0.0F;
        if (iRopeInd >= 0) {
            auto& rope = CRopes::GetRope(iRopeInd);
            // Como el original: _ftol2 completo; el entero de 32 bits se usa en la resta
            nSegments = ObjFtol(rope.m_fSegmentLength * 32.0F);
            fRopeLengthChange = rope.m_fMass * rope.m_fSegmentLength - static_cast<float>(nSegments) * rope.m_fTotalLength;
        }

        // Como el original: MultiplyMatrixWithVector (0x59C890) y después se suma el cambio de longitud a z
        if (m_nModelIndex == ModelIndices::MI_MAGNOCRANE) {
            auto vecRopePoint = ObjMultiplyPoint(*m_matrix, CVector(0.0F, 36.64F, -1.69F));
            vecRopePoint.z += fRopeLengthChange;
            CRopes::RegisterRope((uint32)this, static_cast<uint32>(eRopeType::CRANE_MAGNO), vecRopePoint, false, static_cast<uint8>(nSegments), 1u, this, 20'000u);
        } else if (m_nModelIndex == ModelIndices::MI_CRANETROLLEY) {
            // Como el original: 0 > x elige la bola de demolición; si no (también NaN), la grúa de carro
            const auto nRopeType = static_cast<const uint32>(0.0F > GetPosition().x ? eRopeType::WRECKING_BALL : eRopeType::CRANE_TROLLEY);
            auto vecRopePoint = ObjMultiplyPoint(*m_matrix, CVector(0.0F, 0.0F, 0.0F));
            vecRopePoint.z += fRopeLengthChange;
            CRopes::RegisterRope((uint32)this, nRopeType, vecRopePoint, false, static_cast<uint8>(nSegments), 1u, this, 20'000u);
        } else {
            auto vecRopePoint = ObjMultiplyPoint(*m_matrix, CVector(0.0F, 0.0F, 59.0F));
            vecRopePoint.z += fRopeLengthChange;
            CRopes::RegisterRope((uint32)this, static_cast<uint32>(eRopeType::QUARRY_CRANE_ARM), vecRopePoint, false, static_cast<uint8>(nSegments), 1u, this, 20'000u);
        }
    }

    if (CGarages::IsModelIndexADoor(static_cast<int16>(m_nModelIndex)))
        ProcessGarageDoorBehaviour();
}

// Como el original: índice = (puntero − base) / 0x19C con división entera con signo (trunca hacia cero) y
// comprobación 0 <= índice < tamaño y slot ocupado.
static bool ObjIsInObjectPool(const CObject* object) {
    const auto* pool = reinterpret_cast<const uint8*>(GetObjectPool());
    const auto storage = *reinterpret_cast<const uintptr_t*>(pool + 0x0);
    const auto* byteMap = *reinterpret_cast<const uint8* const*>(pool + 0x4);
    const auto size = *reinterpret_cast<const int32*>(pool + 0x8);
    const auto index = static_cast<int32>(reinterpret_cast<uintptr_t>(object) - storage) / 0x19C;
    if (index < 0 || index >= size)
        return false;

    return !(byteMap[index] & 0x80);
}

// 0x5A2B90
bool IsObjectPointerValid_NotInWorld(CObject* object) {
    return ObjIsInObjectPool(object);
}

// 0x5A2C20
bool IsObjectPointerValid(CObject* object) {
    if (!ObjIsInObjectPool(object))
        return false;

    if (object->m_bIsBIGBuilding)
        return true;

    return !object->m_pCollisionList.IsEmpty();
}
