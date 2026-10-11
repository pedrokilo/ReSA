/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleClimb
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 15/15 entradas.
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
#include "TrigSimpleCpp.h"

#include "TaskSimpleClimb.h"
#include "TaskSimpleClimb_models.h"

#include <reversiblebugfixes/Bugs.hpp>

auto& ms_ClimbColModel    = StaticRef<CColModel>(0xC19518);
auto& ms_StandUpColModel  = StaticRef<CColModel>(0xC19548);
auto& ms_VaultColModel    = StaticRef<CColModel>(0xC19578);
auto& ms_FindEdgeColModel = StaticRef<CColModel>(0xC195A8);

const float ms_fHangingOffsetHorz = -0.40f; // 0x8D2F1C
const float ms_fHangingOffsetVert = -1.10f; // 0x8D2F20

const float ms_fAtEdgeOffsetHorz = -0.40f; // 0x8D2F24
const float ms_fAtEdgeOffsetVert = +0.00f; // 0xC18F78

const float ms_fStandUpOffsetHorz = +0.15f; // 0x8D2F28
const float ms_fStandUpOffsetVert = +1.00f; // 0x8D2F2C

const float ms_fVaultOffsetHorz = +0.50f; // 0x8D2F30
const float ms_fVaultOffsetVert = +0.00f; // 0xC18F7C

namespace {
/*
 * Scalar sine/cosine products now live in TrigSimpleCpp.cpp.
 * Their float cdecl interface matches the former local helpers.
 * Compilation must preserve the extended intermediate result.
 * No float temporary may be stored before the multiplication.
 * The consuming translation unit keeps its existing options.
 * Sine and cosine are both used by GetClimbOffset3D.
 * Its callers include the task and camera calculations below.
 * Native bodies and nested lambdas are compared separately.
 * Compiler-generated temporaries are rejected before runtime tests.
 * The tests retain the existing memory, result and trace contracts.
 * Additional FP or register guarantees require their own evidence.
 * This block preserves existing diagnostic source line numbers
 * while replacing the local helper definitions with shared C++.
 */
// Como el original: fsin/fcos/fpatan del x87 con precisión interna completa; el producto/suma siguiente lo redondea la FPU.
// sin(a) * m

// cos(a) * m

// atan2(-fx, fy) (rumbo de CPlaceable con fpatan)
__declspec(naked) float __cdecl ClimbHeadingOf(float fx, float fy) {
    __asm {
        fld    dword ptr [esp + 4]
        fchs
        fld    dword ptr [esp + 8]
        fpatan
        ret
    }
}

// add + atan2(-fx, fy), con el arcotangente sin redondear
__declspec(naked) float __cdecl ClimbAddHeading(float add, float fx, float fy) {
    __asm {
        fld    dword ptr [esp + 8]
        fchs
        fld    dword ptr [esp + 12]
        fpatan
        fadd   dword ptr [esp + 4]
        ret
    }
}

// val - atan2(-fx, fy), con el arcotangente sin redondear
__declspec(naked) float __cdecl ClimbSubHeading(float val, float fx, float fy) {
    __asm {
        fld    dword ptr [esp + 8]
        fchs
        fld    dword ptr [esp + 12]
        fpatan
        fsubr  dword ptr [esp + 4]
        ret
    }
}

// atan2(-fx, fy) - sub, con el arcotangente sin redondear
__declspec(naked) float __cdecl ClimbHeadingSub(float fx, float fy, float sub) {
    __asm {
        fld    dword ptr [esp + 4]
        fchs
        fld    dword ptr [esp + 8]
        fpatan
        fsub   dword ptr [esp + 12]
        ret
    }
}

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
CVector ClimbMatTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: floor((v * 0.02 + 60)) en doble y truncado (el DLL dividía entre 50)
int32 ClimbSectorIndex(float v) {
    return (int32)std::floor((double)(v * 0.02f + 60.f));
}

// Como el original: la normal cuenta como plana si |x| e |y| no superan 0.05 (NaN incluido)
bool ClimbIsFlatNormal(const CVector& n) {
    return !(n.x > 0.05f) && !(n.x < -0.05f) && !(n.y > 0.05f) && !(n.y < -0.05f);
}

// Como el original: normal orientada hacia el punto (producto 2D con + n.z * 0) y producto con el frente del peatón > 0.3
bool ClimbNormalFacesPed(const CPed* ped, const CColPoint& cp, CVector& outNormal) {
    const auto& pedPos = ped->GetPosition();
    const CVector v{ cp.m_vecPoint.x - pedPos.x, cp.m_vecPoint.y - pedPos.y, cp.m_vecPoint.z - pedPos.z };
    CVector n = cp.m_vecNormal;
    if (v.y * n.y + v.x * n.x + n.z * 0.f < 0.f) {
        n.x *= -1.f;
        n.y *= -1.f;
        n.z *= -1.f;
    }
    outNormal = n;
    const auto& fwd = ped->m_matrix->GetForward();
    return n.y * fwd.y + n.z * fwd.z + n.x * fwd.x > 0.3f;
}
} // namespace

void CTaskSimpleClimb::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClimb, 0x87059C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(ScanToGrabSectorList<CPtrListSingleLink<CPhysical*>>, 0x67DE10);
    RH_ScopedInstall(ScanToGrab, 0x67FD30);
    RH_ScopedInstall(CreateColModel, 0x67A890);
    RH_ScopedInstall(TestForStandUp, 0x680570);
    RH_ScopedInstall(TestForVault, 0x6804D0);
    RH_ScopedInstall(TestForClimb, 0x6803A0);
    RH_ScopedInstall(StartAnim, 0x67DBE0);
    RH_ScopedInstall(StartSpeech, 0x67A320);
    RH_ScopedInstall(DeleteAnimCB, 0x67A380);
    RH_ScopedInstall(Shutdown, 0x67A250);
    RH_ScopedInstall(Constructor, 0x67A110);
    RH_ScopedInstall(GetCameraStickModifier, 0x67A5D0);
    RH_ScopedInstall(GetCameraTargetPos, 0x67A390);
    RH_ScopedVMTInstall(ProcessPed, 0x680DC0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67A280);
}

// 0x67A110
CTaskSimpleClimb::CTaskSimpleClimb(CEntity* climbEntity, const CVector& vecTarget, float fHeading, eSurfaceType nSurfaceType, eClimbHeights nHeight, bool bForceClimb) :
    m_ClimbEntity{ climbEntity },
    m_HandholdPos{ vecTarget },
    m_HandholdHeading{ fHeading },
    m_SurfaceType{ nSurfaceType },
    m_HeightForAnim{ nHeight },
    m_HasToForceClimb{ bForceClimb }
{ }

// 0x67A1D0
CTaskSimpleClimb::~CTaskSimpleClimb() {
    if (const auto a = std::exchange(m_Anim, nullptr)) {
        a->SetDefaultDeleteCallback();
    }
}

// 0x680DC0
bool CTaskSimpleClimb::ProcessPed(CPed* ped) {
    if (m_HasFinished) {
        if (ped->m_pEntityIgnoredCollision == m_ClimbEntity) {
            ped->m_pEntityIgnoredCollision = nullptr;
        }
        return true;
    }

    CEntity* const climbEntity = m_ClimbEntity;
    if (!climbEntity
        || climbEntity->GetIsTypeObject() && !climbEntity->GetIsStatic() && !climbEntity->AsPhysical()->physicalFlags.bDisableCollisionForce
        || climbEntity->GetIsTypeVehicle() && climbEntity->AsVehicle()->IsSubTrain() && [&] {
               // Como el original: módulo 3D en línea ((x*x + y*y) + z*z)
               const auto& s = climbEntity->AsVehicle()->m_vecMoveSpeed;
               return std::sqrt(s.x * s.x + s.y * s.y + s.z * s.z) > 0.1f;
           }()
    ) {
        MakeAbortable(ped);
        return true;
    }

    if (climbEntity->GetIsTypeVehicle()) {
        ped->m_pEntityIgnoredCollision = climbEntity;
    }

    if (!m_Anim || m_HasToChangeAnimation) {
        StartAnim(ped);
        StartSpeech(ped);
    }

    const auto [handPos, handAngle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);

    const bool bNewHeightForPos = m_HasToChangePosition && m_Anim && m_Anim->m_BlendAmount == 1.0f;
    if (bNewHeightForPos) {
        m_HasToChangePosition = false;
        m_HeightForPos        = m_HeightForAnim;
    }

    if (m_HeightForPos > CLIMB_NOT_READY) {
        const auto offset = [&]() -> CVector2D {
            switch (m_HeightForPos) {
            case CLIMB_GRAB:
            case CLIMB_PULLUP: return { ms_fHangingOffsetHorz, ms_fHangingOffsetVert };
            case CLIMB_STANDUP:
            case CLIMB_VAULT: return { ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert };
            case CLIMB_FINISHED: return { ms_fStandUpOffsetHorz, ms_fStandUpOffsetVert };
            case CLIMB_FINISHED_V: return { ms_fVaultOffsetHorz, ms_fVaultOffsetVert };
            default:
                NOTSA_UNREACHABLE();
            }
        }();
        const CVector targetPt = handPos + GetClimbOffset3D(offset, handAngle);

        // Como el original: delta y su módulo al cuadrado ((x*x + y*y) + z*z) antes de la velocidad del portador
        const auto& pedPos = ped->GetPosition();
        CVector delta{ targetPt.x - pedPos.x, targetPt.y - pedPos.y, targetPt.z - pedPos.z };
        const float deltaSqMag = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

        // Como el original: operator/(CVector, float) (0x4119D0) multiplica por 1 / paso
        const auto DivideByTimeStep = [](const CVector& v) {
            const float inv = 1.f / CTimer::GetTimeStep();
            return CVector{ inv * v.x, inv * v.y, inv * v.z };
        };

        CVector vecClimbEntSpeed{};
        if (!climbEntity->GetIsStatic() && climbEntity->GetIsTypePhysical()) {
            vecClimbEntSpeed = climbEntity->AsPhysical()->GetSpeed(targetPt - climbEntity->GetPosition());
        }

        if (bNewHeightForPos) {
            ped->SetPosn(targetPt);
            ped->m_vecMoveSpeed = vecClimbEntSpeed;
        } else if (deltaSqMag < 0.1f) { // Como el original: el paso fino solo por debajo de 0.1 (NaN va al grueso)
            ped->bIsStanding    = false;
            ped->m_vecMoveSpeed = DivideByTimeStep(delta);

            // Como el original: sin limitar la velocidad del paso fino (a muchos FPS puede matar al aterrizar)

            ped->m_vecMoveSpeed += vecClimbEntSpeed;

            if (!(m_Anim->m_Flags & ANIMATION_IS_PLAYING) && m_HeightForAnim == CLIMB_STANDUP) {
                if (TestForVault(ped, handPos, handAngle)) {
                    m_HeightForAnim        = CLIMB_VAULT;
                    m_HasToChangeAnimation = true;
                } else {
                    m_Anim->m_Flags |= ANIMATION_IS_PLAYING;
                }
            }
        } else {
            delta *= 0.25f;
            ped->bIsStanding    = false;
            ped->m_vecMoveSpeed = DivideByTimeStep(delta);

            if (const auto speed = ped->m_vecMoveSpeed.Magnitude(); speed > 0.2f) {
                ped->m_vecMoveSpeed *= 0.2f / speed;
            }

            ped->m_vecMoveSpeed += vecClimbEntSpeed;

            const auto anim = m_Anim;
            if (!(anim->m_Flags & ANIMATION_IS_PLAYING) || anim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
                // Como el original: paso * 0.02 * 1000 truncado y comparaciones con signo de 16 bits
                m_GetToPosCounter += (uint16)(int32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.f);
                if ((int16)m_GetToPosCounter > 1'000 || anim->m_AnimId == ANIM_ID_CLIMB_IDLE && (int16)m_GetToPosCounter > 500) {
                    m_IsInvalidClimb = true;
                    MakeAbortable(ped);
                    ped->ApplyMoveForce(ped->m_matrix->GetForward() * (ped->m_fMass * -0.1F));
                }
            }
        }
    }

    if (bNewHeightForPos) {
        if (notsa::contains({ CLIMB_FINISHED, CLIMB_FINISHED_V }, m_HeightForPos)) {
            m_HasFinished    = true;
            ped->bIsInTheAir = false;
            ped->bIsLanding  = false;

            // Como el original: frente * 0.02 (no / 50) con z - 0.05, o frente * 0.05 con z - 0.01, sin sumar 0 a x e y
            const auto& fwd = ped->m_matrix->GetForward();
            if (m_HeightForPos == CLIMB_FINISHED_V) {
                ped->m_vecMoveSpeed = CVector{ 0.02f * fwd.x, 0.02f * fwd.y, -0.05f + 0.02f * fwd.z };
                ped->bIsStanding    = false;
                ped->bWasStanding   = true;
            } else {
                ped->m_vecMoveSpeed = CVector{ fwd.x * 0.05f, fwd.y * 0.05f, fwd.z * 0.05f - 0.01f };
                ped->bIsStanding    = true;
                ped->bWasStanding   = true;
            }

            if (ped->IsPlayer()) {
                GetEventGlobalGroup()->Add(CEventSoundQuiet{ ped, 50.0f, (uint32)(-1), CVector{} });
            }

            if (m_Anim) {
                m_Anim->SetBlendDelta(-8.0f);
            }

            if (ped->m_pEntityIgnoredCollision == m_ClimbEntity) {
                ped->m_pEntityIgnoredCollision = nullptr;
            }

            return true;
        }
    }

    CPad* const pad = ped->IsPlayer()
        ? ped->AsPlayer()->GetPadFromPlayer()
        : nullptr;

    if (m_Anim && m_Anim->GetBlendAmount() == 1.0f) {
        const auto DoAdvanceAnimHeight = [&]() {
            m_HeightForAnim        = static_cast<eClimbHeights>(m_HeightForAnim + 1);
            m_HasToChangeAnimation = true;
        };
        switch (m_Anim->m_AnimId) {
        case ANIM_ID_CLIMB_JUMP:
            if (m_Anim->m_BlendHier->m_fTotalTime <= m_Anim->m_TimeStep + m_Anim->m_CurrentTime) {
                if (m_HasToForceClimb || pad && pad->GetJump()) {
                    DoAdvanceAnimHeight();
                }
                StartAnim(ped);
            }
            break;
        case ANIM_ID_CLIMB_IDLE:
            if (m_HasToForceClimb || !pad || pad->GetJump()) {
                DoAdvanceAnimHeight();
            }
            break;
        case ANIM_ID_CLIMB_PULL:
        case ANIM_ID_CLIMB_STAND:
            if (m_Anim->m_BlendHier->m_fTotalTime == m_Anim->m_CurrentTime) {
                DoAdvanceAnimHeight();
            }
            break;
        case ANIM_ID_CLIMB_JUMP_B:
            if (m_Anim->m_BlendHier->m_fTotalTime <= m_Anim->m_TimeStep + m_Anim->m_CurrentTime) {
                DoAdvanceAnimHeight();
            }
            break;
        default:
            if (m_Anim->m_BlendHier->m_fTotalTime == m_Anim->m_CurrentTime) {
                if (pad && pad->JumpJustDown() || m_HasToForceClimb) { // Como el original: primero el mando
                    DoAdvanceAnimHeight();
                }
            }
            break;
        }
    }

    if (pad && pad->GetExitVehicle()) {
        MakeAbortable(ped);
    } else if (m_HeightForPos != CLIMB_STANDUP && m_HeightForPos != CLIMB_VAULT || !m_Anim || !(m_Anim->m_Flags & ANIMATION_IS_PLAYING)) {
        if (m_HeightForAnim == CLIMB_STANDUP && m_HeightForPos < CLIMB_STANDUP && TestForVault(ped, handPos, handAngle)) {
            m_HeightForAnim = CLIMB_VAULT;
        }
    } else if (!TestForStandUp(ped, handPos, handAngle)) {
        MakeAbortable(ped);
    }

    ped->m_fAimingRotation = ped->m_fCurrentRotation = handAngle;
    ped->SetOrientation(0.0f, 0.0f, handAngle);

    return false;
}

// 0x67A280
bool CTaskSimpleClimb::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (event && event->GetEventPriority() < 71 && ped->m_fHealth > 0.0F) {
        return false;
    }

    if (m_Anim) {
        m_Anim->m_BlendDelta = -4.0f;
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_Anim = nullptr;
    }

    ped->m_vecMoveSpeed.Set(0.0f, 0.0f, -0.05f);
    m_HasFinished = true;
    return true;
}

// 0x6803A0
CEntity* CTaskSimpleClimb::TestForClimb(CPed* ped, CVector& outClimbPos, float& outClimbHeading, eSurfaceType& outSurfaceType, bool bLaunch) {
    if (const auto entity = ScanToGrab(ped, outClimbPos, outClimbHeading, outSurfaceType, bLaunch, false, false, nullptr)) {
        auto [pt, angle] = GetHandholdPosAndAngleForEntity(entity, outClimbPos, outClimbHeading);
        pt += GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, angle);

        CVector      grabPos;
        eSurfaceType grabSurface;
        float        grabAngle = outClimbHeading;
        if (!ScanToGrab(ped, grabPos, grabAngle, grabSurface, false, true, false, &pt)) {
            return entity;
        }
        outClimbHeading = -9999.9f;
    }
    return nullptr;
}

// 0x67DE10
template<typename PtrListType>
CEntity* CTaskSimpleClimb::ScanToGrabSectorList(PtrListType* sectorList, CPed* ped, CVector& outTargetPos, float& outAngle, eSurfaceType& outSurfaceType, bool isLaunch, bool hasToTestStandup, bool hasToTestDropOtherSide) {
    CEntity* collidedEntity = nullptr;

    // Como el original: nado y modificador de peso se calculan una vez antes del recorrido
    const bool isSwimming = ped->GetIntelligence()->GetTaskSwim() != nullptr;
    int32 fatModifier = 0;
    if (ped->GetPlayerData() && !ped->GetIntelligence()->GetTaskSwim()) {
        fatModifier = (int32)(int64)CStats::GetFatAndMuscleModifier(STAT_MOD_0);
    }

    const auto cm = hasToTestStandup
        ? &ms_StandUpColModel
        : hasToTestDropOtherSide
            ? &ms_VaultColModel
            : &ms_ClimbColModel;
    const CVector cmCentre = ClimbMatTimesPoint(*ped->m_matrix, cm->m_boundSphere.m_vecCenter);
    const float   cmRadius = cm->m_boundSphere.m_fRadius;

    // Como el original: recorrido directo de los nodos (sin validarlos) leyendo el siguiente antes de procesar
    for (auto* node = sectorList->GetNode(); node; ) {
        CEntity* const entity = node->Item;
        node = node->Next;

        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision) {
            continue;
        }

        if (!(entity->GetIsTypeBuilding()
            || (entity->GetIsTypeObject()
                && (entity->GetIsStatic() || entity->AsObject()->physicalFlags.bDisableCollisionForce)
                && !entity->AsObject()->physicalFlags.bInfiniteMass
            )
            || (entity->GetIsTypeVehicle()
                && (hasToTestStandup || hasToTestDropOtherSide || (isSwimming && entity->AsVehicle()->IsSubBoat()) || (entity->AsVehicle()->IsSubTrain() && entity->AsVehicle()->m_vecMoveSpeed.Magnitude() < 0.1F)) // Como el original: módulo 3D (0x4082C0)
            ))) {
            continue;
        }

        // Como el original: distancia sqrt((dx*dx + dz*dz) + dy*dy) desde el centro transformado; NaN no pasa
        {
            const CVector bc = entity->GetBoundCentre();
            const float dx = cmCentre.x - bc.x, dy = cmCentre.y - bc.y, dz = cmCentre.z - bc.z;
            const float dist = std::sqrt(dx * dx + dz * dz + dy * dy);
            if (!(entity->GetModelInfo()->GetColModel()->GetBoundRadius() + cmRadius > dist)) {
                continue;
            }
        }

        // Como el original: palabra baja de un dword a -1 (comparación con signo de 16 bits)
        int16 numSpheres = -1;
        if (entity->GetIsTypeVehicle() && entity->AsVehicle()->IsSubBoat()) {
            numSpheres                                       = (int16)entity->GetColModel()->m_pColData->m_nNumSpheres;
            entity->GetColModel()->m_pColData->m_nNumSpheres = 0;
        }
        const auto RestoreSpheres = [&] {
            if (entity->GetIsTypeVehicle() && numSpheres > -1) {
                entity->GetColModel()->m_pColData->m_nNumSpheres = (uint16)numSpheres;
            }
        };

        CMatrix& entityMatrix = entity->GetMatrix();
        if (CCollision::ProcessColModels(*ped->m_matrix, *cm, entityMatrix, *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) <= 0) {
            RestoreSpheres();
            continue;
        }

        if (hasToTestStandup || hasToTestDropOtherSide) {
            RestoreSpheres();
            return entity;
        }

        const auto& cp = CWorld::m_aTempColPts[0];
        const CVector relPosn{ cp.m_vecPoint.x - outTargetPos.x, cp.m_vecPoint.y - outTargetPos.y, cp.m_vecPoint.z - outTargetPos.z };
        const uint8 nColSphereIndex = cp.m_nPieceTypeA;
        const auto IsReturnSphere = [&] {
            switch (nColSphereIndex) {
            case 0: case 1: case 2: case 3: case 4: case 10:
                return true;
            }
            return false;
        };

        if (nColSphereIndex == 0 || nColSphereIndex == 1 || nColSphereIndex == 2 || nColSphereIndex == 3) {
            RestoreSpheres();
            return (CEntity*)(1);
        }

        // Como el original: orden altura/frente, CanClimb y después las esferas 4, 10 y 16
        bool skip = false;
        if (cp.m_vecPoint.z <= outTargetPos.z) {
            const auto& fwd = ped->m_matrix->GetForward();
            if (relPosn.z * fwd.z + relPosn.y * fwd.y + relPosn.x * fwd.x >= 0.0f) {
                skip = true;
            }
        }
        if (!skip && !g_surfaceInfos.CanClimb(cp.m_nSurfaceTypeB)) {
            skip = true;
        }
        if (!skip && (nColSphereIndex == 4 || nColSphereIndex == 10 || nColSphereIndex == 16)) {
            skip = true;
        }
        if (skip) {
            RestoreSpheres();
            if (IsReturnSphere()) {
                return (CEntity*)(1);
            }
            continue;
        }

        if (!isLaunch || fatModifier >= 1) {
            if (nColSphereIndex == 5 || nColSphereIndex == 11 || nColSphereIndex == 17) {
                RestoreSpheres();
                continue;
            }

            if (fatModifier >= 1 && (!isLaunch || fatModifier >= 2)) {
                if (nColSphereIndex == 6 || nColSphereIndex == 12 || nColSphereIndex == 18) {
                    RestoreSpheres();
                    continue;
                }
            }
        }

        if (ClimbIsFlatNormal(cp.m_vecNormal)) {
            outTargetPos   = cp.m_vecPoint;
            outAngle       = ped->m_fCurrentRotation;
            outSurfaceType = cp.m_nSurfaceTypeB;
            collidedEntity = entity;
        } else if (CVector n; ClimbNormalFacesPed(ped, cp, n)) {
            outTargetPos   = cp.m_vecPoint;
            outAngle       = ClimbHeadingOf(n.x, n.y);
            outSurfaceType = cp.m_nSurfaceTypeB;
            collidedEntity = entity;
        }

        // Como el original: matriz del peatón trasladada al punto de contacto
        CMatrix matrix{ *ped->m_matrix };
        matrix.SetTranslateOnly(cp.m_vecPoint);
        if (CCollision::ProcessColModels(matrix, ms_FindEdgeColModel, entity->GetMatrix(), *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
            const auto& edge = CWorld::m_aTempColPts[0];
            if (ClimbIsFlatNormal(edge.m_vecNormal)) {
                outTargetPos   = edge.m_vecPoint;
                outSurfaceType = edge.m_nSurfaceTypeB;
            } else if (CVector n; ClimbNormalFacesPed(ped, edge, n)) {
                outTargetPos   = edge.m_vecPoint;
                collidedEntity = entity;
                outAngle       = ClimbHeadingOf(n.x, n.y);
                outSurfaceType = edge.m_nSurfaceTypeB;
            }
        }

        RestoreSpheres();
    }

    return collidedEntity;
}

// 0x67FD30
CEntity* CTaskSimpleClimb::ScanToGrab(CPed* ped, CVector& outClimbPos, float& outClimbAngle, eSurfaceType& pSurfaceType, bool flag1, bool bStandUp, bool bVault, CVector* pedPosition) {
    if (!ms_ClimbColModel.m_pColData) {
        CreateColModel();
    }

    const CVector originalPedPosition = ped->GetPosition();

    if (pedPosition) {
        ped->SetPosn(*pedPosition);
    }

    // Como el original: posición + frente * 10 con la matriz del peatón
    outClimbPos = ped->GetPosition();
    {
        const auto& fwd = ped->m_matrix->GetForward();
        outClimbPos.x = fwd.x * 10.0f + outClimbPos.x;
        outClimbPos.y = fwd.y * 10.0f + outClimbPos.y;
        outClimbPos.z = fwd.z * 10.0f + outClimbPos.z;
    }

    const auto  outPoint = ClimbMatTimesPoint(*ped->m_matrix, ms_ClimbColModel.m_boundSphere.m_vecCenter);
    const float radius   = ms_ClimbColModel.m_boundSphere.m_fRadius;

    const int32 startSectorX = ClimbSectorIndex(outPoint.x - radius);
    const int32 startSectorY = ClimbSectorIndex(outPoint.y - radius);
    const int32 endSectorX   = ClimbSectorIndex(radius + outPoint.x);
    const int32 endSectorY   = ClimbSectorIndex(radius + outPoint.y);

    CWorld::AdvanceCurrentScanCode();

    CEntity* collidedEntity = nullptr;
    for (int32 y = startSectorY; y <= endSectorY; y++) {
        for (int32 x = startSectorX; x <= endSectorX; x++) {
            // Como el original: el sector se recorta a [0, 119] y el sector repetido usa (x & 15, y & 15)
            auto& repeatSector = CWorld::ms_aRepeatSectors[(size_t)(y & 15)][(size_t)(x & 15)];
            auto* const scanResult1 = ScanToGrabSectorList(&CWorld::GetSector(x, y).Buildings, ped, outClimbPos, outClimbAngle, pSurfaceType, flag1, bStandUp, bVault);
            CEntity* scanResult2 = ScanToGrabSectorList(&repeatSector.Objects, ped, outClimbPos, outClimbAngle, pSurfaceType, flag1, bStandUp, bVault);
            if (!scanResult2) {
                scanResult2 = ScanToGrabSectorList(&repeatSector.Vehicles, ped, outClimbPos, outClimbAngle, pSurfaceType, flag1, bStandUp, bVault);
            }

            if (scanResult1 == (CEntity*)(1) || scanResult2 == (CEntity*)(1)) {
                return nullptr; // Como el original: sin restaurar la posición del peatón
            }

            if (auto entity = scanResult2 ? scanResult2 : scanResult1) {
                if (bStandUp || bVault) {
                    if (pedPosition) {
                        ped->SetPosn(originalPedPosition);
                    }

                    return entity;
                } else {
                    collidedEntity = entity;
                }
            }
        }
    }

    if (pedPosition) {
        ped->SetPosn(originalPedPosition);
    }

    if (collidedEntity) {
        if (collidedEntity->GetIsTypePhysical()) {
            // Como el original: inversa por 0x59C890 y rumbo restado con fpatan sin redondear
            const CMatrix& entityMatrix = collidedEntity->GetMatrix();
            outClimbPos   = ClimbMatTimesPoint(Invert(entityMatrix), outClimbPos);
            outClimbAngle = ClimbSubHeading(outClimbAngle, entityMatrix.GetForward().x, entityMatrix.GetForward().y);
        }
    }

    return collidedEntity;
}

// 0x67A890
bool CTaskSimpleClimb::CreateColModel() {
    // Como el original: cada esfera con CColSphere::Set(radio, centro, material, pieza, 0xFF) (0x40FD10), sin tocar el último byte de la superficie
    const auto SetSpheres = [](CColModel& cm, const auto& spheres) {
        for (size_t i = 0; i < std::size(spheres); i++) {
            cm.m_pColData->m_pSpheres[i].Set(spheres[i].m_fRadius, spheres[i].m_vecCenter, spheres[i].m_Surface.m_nMaterial, spheres[i].m_Surface.m_nPiece, tColLighting(0xFF));
        }
    };

    ms_ClimbColModel.AllocateData((int32)std::size(ClimbColModelSpheres), 0, 0, 0, 0, false);
    SetSpheres(ms_ClimbColModel, ClimbColModelSpheres);
    ms_ClimbColModel.m_boundSphere.Set(2.02f, { 0.0f, 0.75f, 1.65f });
    ms_ClimbColModel.GetBoundingBox().Set({ -0.4f, -0.46f, 0.0f }, { 0.4f, 1.9f, 3.3f });
    ms_ClimbColModel.m_nColSlot = 0;

    ms_StandUpColModel.AllocateData((int32)std::size(StandUpColModelSpheres), 0, 0, 0, 0, false);
    SetSpheres(ms_StandUpColModel, StandUpColModelSpheres);
    ms_StandUpColModel.m_boundSphere.Set(1.5f, { 0.0f, 0.3f, 0.7f });
    ms_StandUpColModel.GetBoundingBox().Set({ -0.35f, -0.35f, -0.65f }, { 0.35f, 0.95f, 2.1f });
    ms_StandUpColModel.m_nColSlot = 0;

    ms_VaultColModel.AllocateData((int32)std::size(VaultColModelSpheres), 0, 0, 0, 0, false);
    SetSpheres(ms_VaultColModel, VaultColModelSpheres);
    ms_VaultColModel.m_boundSphere.Set(1.15f, { 0.0f, 0.5f, 0.1f });
    ms_VaultColModel.GetBoundingBox().Set({ -0.35f, -0.35f, -0.6f }, { 0.35f, 1.3f, 0.85f });
    ms_VaultColModel.m_nColSlot = 0;

    ms_FindEdgeColModel.AllocateData((int32)std::size(FindEdgeColModelSpheres), 0, 0, 0, 0, false);
    SetSpheres(ms_FindEdgeColModel, FindEdgeColModelSpheres);
    ms_FindEdgeColModel.m_boundSphere.Set(0.9f, { 0.0f, -0.2f, 0.2f });
    ms_FindEdgeColModel.GetBoundingBox().Set({ -0.3f, -0.8f, -0.4f }, { 0.3f, 0.4f, 0.8f });
    ms_FindEdgeColModel.m_nColSlot = 0;

    return true;
}

// 0x67A250
void CTaskSimpleClimb::Shutdown() {
    ms_ClimbColModel.RemoveCollisionVolumes();
    ms_StandUpColModel.RemoveCollisionVolumes();
    ms_VaultColModel.RemoveCollisionVolumes();
    ms_FindEdgeColModel.RemoveCollisionVolumes();
}

// 0x680570
bool CTaskSimpleClimb::TestForStandUp(CPed* ped, const CVector& point, float fAngle) {
    CVector      climbPos;
    float        angle;
    eSurfaceType surfaceType;
    CVector      pedPos = point + GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, fAngle);
    return !ScanToGrab(ped, climbPos, angle, surfaceType, false, true, false, &pedPos);
}

// 0x6804D0
bool CTaskSimpleClimb::TestForVault(CPed* ped, const CVector& point, float fAngle) {
    CVector      climbPos;
    float        angle;
    eSurfaceType surfaceType;
    CVector      pedPos = point + GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, fAngle);
    return !m_ClimbEntity->GetIsTypeVehicle()
        && !ScanToGrab(ped, climbPos, angle, surfaceType, false, false, true, &pedPos);
}

// 0x67DBE0
void CTaskSimpleClimb::StartAnim(CPed* ped) {
    switch (m_HeightForAnim) {
    case CLIMB_GRAB:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_IDLE, 4.0f);
        } else {
            m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0f);
        }
        m_HeightForPos    = CLIMB_GRAB;
        m_HasToChangePosition  = false;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_PULLUP:
        if (m_HeightForPos == CLIMB_NOT_READY) {
            m_HeightForAnim = CLIMB_STANDUP;
            m_HeightForPos  = CLIMB_STANDUP;
            m_Anim          = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 4.0f);
            m_Anim->m_Flags &= ~ANIMATION_IS_PLAYING;
        } else {
            if (m_Anim) { // Como el original: comprueba la animación antes de soltarla
                m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            }
            m_Anim         = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_PULL, 1000.0f);
            m_HeightForPos = CLIMB_PULLUP;
        }
        m_HasToChangePosition  = false;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_STANDUP:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim            = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 1000.0f);
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_FINISHED:
    case CLIMB_FINISHED_V:
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        ped->SetMoveState(PEDMOVE_STILL);
        ped->SetMoveAnim();
        if (ped->GetPlayerData()) {
            ped->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
        }
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim = CAnimManager::BlendAnimation(
            ped->GetRpClump(), ANIM_GROUP_DEFAULT, m_HeightForAnim == CLIMB_FINISHED_V ? ANIM_ID_CLIMB_JUMP2FALL : ANIM_ID_CLIMB_STAND_FINISH, 1000.0F
        );
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_VAULT:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim            = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP_B, m_Anim && m_Anim->m_AnimId == ANIM_ID_CLIMB_STAND ? 16.0f : 1000.0f);
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    default:
        break;
    }

    if (m_Anim) {
        m_Anim->SetDeleteCallback(DeleteAnimCB, this);
        if (ped->GetPlayerData() && m_Anim->m_Flags & ANIMATION_IS_PLAYING && (m_Anim->m_AnimId == ANIM_ID_CLIMB_PULL || m_Anim->m_AnimId == ANIM_ID_CLIMB_STAND || m_Anim->m_AnimId == ANIM_ID_CLIMB_JUMP_B)) {
            m_Anim->m_Speed = CStats::GetFatAndMuscleModifier(STAT_MOD_1);
        }
    }
}

// 0x67A320
void CTaskSimpleClimb::StartSpeech(CPed* ped) const {
    if (ped->IsPlayer()) {
        if (m_HeightForAnim == CLIMB_PULLUP) {
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN);
        } else if (m_HeightForAnim == CLIMB_STANDUP) {
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN_EXHALE);
        }
    }
}

// 0x67A380
void CTaskSimpleClimb::DeleteAnimCB(CAnimBlendAssociation* anim, void* data) {
    static_cast<CTaskSimpleClimb*>(data)->m_Anim = nullptr; // Como el original: sin comprobar el tipo
}

// 0x67A5D0
void CTaskSimpleClimb::GetCameraStickModifier(CEntity* entity, float& outAlpha, float& outBeta, float& outStickAlpha, float& outStickBeta) {
    if (!m_Anim) {
        return;
    }

    if (m_Anim->m_AnimId == ANIM_ID_CLIMB_JUMP_B) {
        // Como el original: la caída tras el salto se calcula una sola vez (mientras sea negativa) y con el desplazamiento de salto
        if (m_FallAfterVault < 0) {
            auto [pos, angle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);
            pos += GetClimbOffset3D({ ms_fVaultOffsetHorz, ms_fVaultOffsetVert }, angle);

            CColPoint colPoint{};
            CEntity*  colEntity{};
            m_FallAfterVault = CWorld::ProcessVerticalLine(pos, pos.z - 3.0f, colPoint, colEntity, true, true, false, true, false, false, nullptr)
                ? (int8)(int32)(int64)(std::max(pos.z - colPoint.m_vecPoint.z - 1.0f, 0.0F) * 10.0f)
                : 20;
        }

        const float d = m_FallAfterVault > 8 ? -0.6f : -0.1f;
        if (outAlpha > d) {
            const float stick = (outAlpha - d) * -0.05f;
            outStickAlpha = -0.05f > stick ? -0.05f : stick; // Como el original: NaN se conserva
        }
    } else if (m_Anim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
        // Como el original: rumbo de la entidad del argumento (fpatan sin redondear) y constantes del EXE
        constexpr float halfPi        = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
        constexpr float pi            = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
        constexpr float twoPi         = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
        constexpr float halfPiPlus02  = std::bit_cast<float>(0x3FE2A975u); // 0x8705C0
        float fHeading = entity->m_matrix
            ? ClimbHeadingSub(entity->m_matrix->GetForward().x, entity->m_matrix->GetForward().y, halfPi)
            : entity->m_placement.m_fHeading - halfPi;
        if (fHeading > outBeta + pi) {
            fHeading -= twoPi;
        } else if (fHeading < outBeta - pi) {
            fHeading += twoPi;
        }

        if (fHeading > outBeta + halfPiPlus02) {
            outStickBeta = 0.02f;
        } else if (fHeading > outBeta + halfPi) {
            if (outStickBeta < 0.0F) {
                outStickBeta = 0.0f;
            }
        } else if (fHeading < outBeta - halfPi - 0.2F) {
            outStickBeta = -0.02f;
        } else if (fHeading < outBeta - halfPi) {
            if (outStickBeta > 0.0F) {
                outStickBeta = 0.0f;
            }
        }
    }
}

// 0x67A390
void CTaskSimpleClimb::GetCameraTargetPos(CPed* ped, CVector& outTargetPt) {
    outTargetPt = ped->GetPosition();

    if (!m_Anim) {
        return;
    }

    // Calculate offset (Como el original: lerp de (horizontal, vertical) entre dos desplazamientos: A * (1 - t) + B * t)
    const float t = m_Anim->m_CurrentTime / m_Anim->m_BlendHier->m_fTotalTime;
    const float c = 1.0f - t;
    CVector2D offset{};
    switch (m_Anim->m_AnimId) {
    case ANIM_ID_FALL_GLIDE:
    case ANIM_ID_CLIMB_JUMP2FALL:    offset = { ms_fVaultOffsetHorz, ms_fVaultOffsetVert };                                                                      break;
    case ANIM_ID_CLIMB_PULL:         offset = { ms_fHangingOffsetHorz * c + ms_fAtEdgeOffsetHorz * t, c * ms_fHangingOffsetVert + ms_fAtEdgeOffsetVert * t }; break;
    case ANIM_ID_CLIMB_STAND:        offset = { ms_fAtEdgeOffsetHorz * c + ms_fStandUpOffsetHorz * t, c * ms_fAtEdgeOffsetVert + ms_fStandUpOffsetVert * t }; break;
    case ANIM_ID_CLIMB_STAND_FINISH: offset = { ms_fStandUpOffsetHorz, ms_fStandUpOffsetVert };                                                                  break;
    case ANIM_ID_CLIMB_JUMP_B:       offset = { ms_fAtEdgeOffsetHorz * c + ms_fVaultOffsetHorz * t, c * ms_fAtEdgeOffsetVert + ms_fVaultOffsetVert * t };     break;
    default:                         return;
    }

    // Calculate final target position
    const auto [point, angle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);
    outTargetPt = point + GetClimbOffset3D(offset, angle);
}

// Como el original: (-sin(a) * h, cos(a) * h, v) con fsin/fcos sin redondear
CVector CTaskSimpleClimb::GetClimbOffset3D(CVector2D offset2D, float angle) {
    return CVector{
        -TrigSimpleSinMulCpp(angle, offset2D.x),
        TrigSimpleCosMulCpp(angle, offset2D.x),
        offset2D.y
    };
}

// Como el original: matriz asegurada (GetMatrix), punto por 0x59C890 y rumbo con fpatan sumado sin redondear
std::pair<CVector, float> CTaskSimpleClimb::GetHandholdPosAndAngleForEntity(CEntity* entity, const CVector& handPos, float handAngle) {
    if (entity->GetIsTypePhysical()) {
        const CMatrix& matrix = entity->GetMatrix();
        const CVector  point  = ClimbMatTimesPoint(matrix, handPos);
        return { point, ClimbAddHeading(handAngle, matrix.GetForward().x, matrix.GetForward().y) };
    }
    return { handPos, handAngle };
}
