/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSwim
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TrigCompound6Cpp.h"
#include "TrigSimpleCpp.h"

#include "TaskSimpleSwim.h"
#include "TaskSimpleClimb.h"

void CTaskSimpleSwim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSwim, 0x870920, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x688930);
    RH_ScopedVMTInstall(ProcessPed, 0x68B1C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68B100);
    RH_ScopedInstall(ApplyRollAndPitch, 0x68A8E0);
    RH_ScopedInstall(ProcessSwimAnims, 0x6899F0);
    RH_ScopedInstall(ProcessSwimmingResistance, 0x68A1D0);
    RH_ScopedInstall(ProcessEffects, 0x68AA70);
    RH_ScopedInstall(ProcessControlAI, 0x689640);
    RH_ScopedInstall(ProcessControlInput, 0x688A90);
    RH_ScopedInstall(CreateFxSystem, 0x68A9F0);
    RH_ScopedInstall(DestroyFxSystem, 0x68AA50);
}
CTaskSimpleSwim* CTaskSimpleSwim::Constructor(CVector* pos, CPed* ped) { this->CTaskSimpleSwim::CTaskSimpleSwim(pos, ped); return this; }

// 0x688930
CTaskSimpleSwim::CTaskSimpleSwim(const CVector* pos, CPed* ped) :
    m_vecPos{pos ? *pos : CVector{}}
{
    m_bFinishedBlending = false;
    m_bAnimBlockRefAdded = false;
    m_fAnimSpeed = -1.0f;
    m_pPed = ped;
    m_fRotationX = 0.0f;
    m_fTurningRotationY = 0.0f;
    m_fUpperTorsoRotationX = 0.0f;
    m_fAimingRotation = 0.0f;
    m_fStateChanger = 0.0f;
    m_pEntity = nullptr;
    m_fSwimStopTime = 0.0f;
    m_nTimeStep = 0;
    m_nSwimState = SWIM_TREAD;
    m_AnimID = ANIM_ID_NO_ANIMATION_SET;

    CEntity::SafeRegisterRef(m_pPed);

    m_pFxSystem = nullptr;
    m_bTriggerWaterSplash = false;
    m_fRandomMoveBlendRatio = 0.0f;
}

// 0x688A00
CTaskSimpleSwim::~CTaskSimpleSwim() {
    if (m_bAnimBlockRefAdded) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(ANIM_GROUP_SWIM));
    }

    CEntity::SafeCleanUpRef(m_pEntity); // todo: m_pEntity not registered CEntity::SafeRegisterRef() in ctor
    CEntity::SafeCleanUpRef(m_pPed);
}

namespace {
// Como el original: constantes exactas del EXE (PI/DegreesToRadians de common.h son imprecisos)
constexpr float SWIM_PI           = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float SWIM_NEG_PI       = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
constexpr float SWIM_TWO_PI       = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
constexpr float SWIM_MAX_PITCH    = std::bit_cast<float>(0x3FB2B8C3u); // 0x870948 (80 grados)
constexpr float SWIM_QUARTER_PI   = std::bit_cast<float>(0x3F490FDBu); // 0x859AB0
constexpr float SWIM_FOUR_OVER_PI = std::bit_cast<float>(0x3FA2F983u); // 0x870950
constexpr float SWIM_DEG_TO_RAD   = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC
constexpr float SWIM_THIRD        = std::bit_cast<float>(0x3EAAAAABu); // 0x859040
constexpr float SWIM_POW_09       = 0.9f;                              // 0x8708D0

// Como el original: variables globales del EXE (no constantes)
auto& SwimDiveUnderAngle  = StaticRef<float>(0x8D2FC4); // -35
auto& SwimStopTime        = StaticRef<float>(0x8D2FC0); // 15
auto& SwimAnimShiftFactor = StaticRef<float>(0x8D2FF4); // 0,5

float WrapSwimAngle(float a) {
    if (a > SWIM_PI) {
        return a - SWIM_TWO_PI;
    }
    if (a < SWIM_NEG_PI) {
        return a + SWIM_TWO_PI;
    }
    return a;
}

// fpatan(y, x) con el resultado redondeado a float al guardarlo, como el EXE
float __declspec(naked) __cdecl Atan2X87(float y, float x) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        fpatan
        ret
    }
}

// fpatan(y, x) guardado en *out y restado a la precisión vigente con el valor completo de ST0
float __declspec(naked) __cdecl Atan2SubX87(float y, float x, float sub, float* out) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        fpatan
        mov    eax, dword ptr [esp + 10h]
        fst    dword ptr [eax]
        fsub   dword ptr [esp + 0Ch]
        ret
    }
}

// asin del CRT del EXE (_CIasin 0x821E70) menos sub con el valor completo de ST0
float __declspec(naked) __cdecl AsinSubX87(float x, float sub) {
    __asm {
        fld    dword ptr [esp + 4]
        mov    eax, 0x821E70
        call   eax
        fsub   dword ptr [esp + 8]
        ret
    }
}

// pow del CRT del EXE (_CIpow 0x822130) por v con el valor completo de ST0
float __declspec(naked) __cdecl MulPowX87(float base, float exp, float v) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        mov    eax, 0x822130
        call   eax
        fmul   dword ptr [esp + 0Ch]
        ret
    }
}





// z - fsin(a) a la precisión vigente


// out[0..2] = fcos(a) * f[0..2], out[3] = fsin(a) * m (productos con el valor completo de ST0)
void __declspec(naked) __cdecl CosSinProductsX87(float a, const float* f, float m, float* out) {
    __asm {
        mov    eax, dword ptr [esp + 8]
        mov    edx, dword ptr [esp + 10h]
        fld    dword ptr [esp + 4]
        fcos
        fld    st(0)
        fmul   dword ptr [eax]
        fstp   dword ptr [edx]
        fld    st(0)
        fmul   dword ptr [eax + 4]
        fstp   dword ptr [edx + 4]
        fmul   dword ptr [eax + 8]
        fstp   dword ptr [edx + 8]
        fld    dword ptr [esp + 4]
        fsin
        fmul   dword ptr [esp + 0Ch]
        fstp   dword ptr [edx + 0Ch]
        ret
    }
}

const auto s_RadianAngleBetweenPoints = static_cast<float (*)(float, float, float, float)>(&CGeneral::GetRadianAngleBetweenPoints);

// CGeneral::GetRadianAngleBetweenPoints(0, 0, x2, y2) - sub con el valor completo de ST0 (0x53CBE0 devuelve fpatan sin redondear)
float __declspec(naked) __cdecl RadianAngleFromOriginSubX87(float x2, float y2, float sub) {
    __asm {
        push   dword ptr [esp + 8]
        push   dword ptr [esp + 8]
        push   0
        push   0
        call   dword ptr [s_RadianAngleBetweenPoints]
        add    esp, 10h
        fsub   dword ptr [esp + 0Ch]
        ret
    }
}

// ms = pow(0,9, paso) * ms + (1 - pow) * v con la secuencia x87 del EXE (0x68A4D8)
void ApplySwimResistanceX87(float step, CVector* ms, const CVector* v) {
    float t0, t1, t2;
    const float base = SWIM_POW_09;
    __asm {
        fld    base
        fld    step
        mov    eax, 0x822130
        call   eax
        fld1
        fsub   st, st(1)
        mov    edx, v
        fld    dword ptr [edx]
        fmul   st, st(1)
        fstp   t0
        fld    dword ptr [edx + 4]
        fmul   st, st(1)
        fstp   t1
        fmul   dword ptr [edx + 8]
        mov    ecx, ms
        fld    st(1)
        fmul   dword ptr [ecx]
        fld    st(2)
        fmul   dword ptr [ecx + 4]
        fxch   st(3)
        fmul   dword ptr [ecx + 8]
        fstp   t2
        fadd   t0
        fstp   dword ptr [ecx]
        fxch   st(1)
        fadd   t1
        fstp   dword ptr [ecx + 4]
        fld    t2
        fadd   st, st(1)
        fstp   dword ptr [ecx + 8]
        fstp   st(0)
    }
}
} // namespace

// 0x68A9F0
void CTaskSimpleSwim::CreateFxSystem(CPed* ped, RwMatrix* mat) {
    CVector point{};
    m_pFxSystem = g_fxMan.CreateFxSystem("water_ripples", point, mat, false);
    if (m_pFxSystem) {
        m_pFxSystem->CopyParentMatrix();
        m_pFxSystem->Play();
    }
}

// 0x68AA50
void CTaskSimpleSwim::DestroyFxSystem() {
    FxSystem_c::SafeKillAndClear(m_pFxSystem);
}

// 0x68B100
bool CTaskSimpleSwim::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        ped->m_nMoveState = PEDMOVE_STILL;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;

        if (m_AnimID != ANIM_ID_NO_ANIMATION_SET) {
            auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_AnimID);
            if (assoc) {
                assoc->m_BlendDelta = -1000.0f;
            }
        }

        ped->RestoreHeadingRate();

        DestroyFxSystem();
        return true;
    }

    if (!event) {
        return false;
    }
    const auto* dmgEvent = static_cast<const CEventDamage*>(event);
    if (event->GetEventPriority() >= 71 || event->GetEventType() == EVENT_DAMAGE && dmgEvent->m_damageResponse.m_bHealthZero && dmgEvent->m_bAddToEventGroup) {
        DestroyFxSystem();
        return true;
    }
    return false;
}

// 0x68B1C0
bool CTaskSimpleSwim::ProcessPed(CPed* ped) {
    if (m_pEntity) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 8.0f);
        ped->m_nMoveState = PEDMOVE_STILL;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;

        DestroyFxSystem();
        return true;
    }

    // Como el original: el límite es la variable global 0x8D2FC0
    if (m_fSwimStopTime > SwimStopTime || ped->bInVehicle) {
        CAnimBlendAssociation* assoc = nullptr;
        if (m_AnimID != ANIM_ID_NO_ANIMATION_SET) {
            assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_AnimID);
        }
        AnimationId animId = ANIM_ID_IDLE;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;
        ped->m_nMoveState = PEDMOVE_STILL;
        if (assoc) {
            if (assoc->m_AnimId == ANIM_ID_CLIMB_JUMP)
                assoc->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
            else
                assoc->m_BlendDelta = -4.0f;

            if (m_AnimID == ANIM_ID_SWIM_BREAST) {
                animId = ANIM_ID_WALK;
                ped->m_nSwimmingMoveState = PEDMOVE_WALK;
                ped->m_nMoveState = PEDMOVE_WALK;
            } else if (m_AnimID == ANIM_ID_SWIM_CRAWL) {
                animId = ANIM_ID_RUN;
                ped->m_nSwimmingMoveState = PEDMOVE_RUN;
                ped->m_nMoveState = PEDMOVE_RUN;
            }
        }
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, animId, 4.0f);
        ped->RestoreHeadingRate();
        DestroyFxSystem();
        return true;
    }

    ped->bIsInTheAir = false;

    if (ped->IsPlayer()) {
        if (m_nTimeStep && m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            ProcessControlAI(ped);
            // Como el original: paso · 0,02 · 1000 truncado con _ftol2
            const auto swimmingTimeStep = static_cast<uint32>(static_cast<int64>(CTimer::GetTimeStep() * 0.02f * 1000.0f));
            if (m_nTimeStep <= swimmingTimeStep)
                m_nTimeStep = 0;
            else
                m_nTimeStep -= swimmingTimeStep;

            ped->GetPlayerData()->m_fMoveBlendRatio = DistanceBetweenPoints2D(ped->GetPosition(), m_vecPos);
            if (ped->GetPlayerData()->m_fMoveBlendRatio < 0.5f) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
                CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 4.0f);
                ped->RestoreHeadingRate();
                DestroyFxSystem();
                return true;
            }
            if (ped->GetPlayerData()->m_fMoveBlendRatio > 1.0f) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 1.0f;
            }
        } else {
            ProcessControlInput(ped->AsPlayer());
            m_nTimeStep = 0;
        }

        bool bDecreaseAir = false;
        float fDecreaseAirMult = 1.0f;
        if (m_nSwimState == SWIM_UNDERWATER_SPRINTING) {
            bDecreaseAir = true;
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_UNDER);
            if (assocUnder) {
                fDecreaseAirMult = assocUnder->m_Speed * assocUnder->m_BlendAmount + 1.0f;
            }
        }

        ped->AsPlayer()->HandlePlayerBreath(bDecreaseAir, fDecreaseAirMult);
        // Como el original: comprueba m_bFinishedBlending (byte +0x8), no m_pPed
        if (m_bFinishedBlending && m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 0.5f > ped->GetPlayerData()->m_fBreath) {
                ped->Say(CTX_GLOBAL_PAIN_CJ_SWIM_GASP);
            }
        }
        ped->SetMoveState(PEDMOVE_NONE);
    } else {
        ProcessControlAI(ped);

        if (m_nSwimState == SWIM_UNDERWATER_SPRINTING) {
            CPedDamageResponseCalculator damageCalculator(nullptr, CTimer::GetTimeStep(), WEAPON_DROWNING, PED_PIECE_TORSO, false);
            CEventDamage eventDamage(nullptr, CTimer::GetTimeInMS(), WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, ped->bInVehicle);
            if (eventDamage.AffectsPed(ped))
                damageCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);
            else
                eventDamage.m_damageResponse.m_bDamageCalculated = true;
            ped->GetEventGroup().Add(&eventDamage, false);
        }
    }

    ProcessSwimAnims(ped);
    ProcessSwimmingResistance(ped);
    ProcessEffects(ped);
    return false;
}

// 0x68A8E0
void CTaskSimpleSwim::ApplyRollAndPitch(CPed* ped) const {
    LimbOrientation orientation(m_fAimingRotation, m_fUpperTorsoRotationX);
    ped->m_pedIK.RotateTorso(ped->m_apBones[PED_NODE_UPPER_TORSO], orientation, rwCOMBINEREPLACE);
    if (ped->GetRwObject()) {
        CMatrix pedMatrix(ped->GetModellingMatrix(), false);
        CMatrix rotationMatrix;
        rotationMatrix.SetTranslate(CVector{});
        // Como el original: CMatrix::RotateY (0x59B2C0) y luego RotateX (0x59B1E0)
        rotationMatrix.RotateY(m_fTurningRotationY);
        rotationMatrix.RotateX(m_fRotationX);
        pedMatrix *= rotationMatrix;
        pedMatrix.UpdateRW();
        ped->UpdateRwFrame();
    }
}

// 0x6899F0
void CTaskSimpleSwim::ProcessSwimAnims(CPed* ped) {
    auto* player = ped->AsPlayer();
    const CVector& pedPos = player->GetPosition();
    CAnimBlendAssociation* assocTread = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_TREAD);
    if (m_bFinishedBlending) {
        if (assocTread && assocTread->m_BlendAmount < 1.0f && assocTread->m_BlendDelta <= 0.0f) {
            CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.0f);
        }
    } else {
        if (assocTread || (assocTread = CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.0f)) != nullptr) {
            if (assocTread->m_BlendAmount >= 1.0f) {
                m_bFinishedBlending = true;
            }
        }
        RpAnimBlendClumpSetBlendDeltas(player->GetRpClump(), 0x10, -8.0f); // todo: ANIMATION_IS_PARTIAL ?
        FxSystem_c::SafeKillAndClear(player->GetActiveWeapon().m_FxSystem); // Removes fire or something in water

        if (player->IsPlayer() && !m_nSwimState) {
            float waterLevel = 0.0f;
            if (CWaterLevel::GetWaterLevel(pedPos, waterLevel, true)) {
                if (waterLevel - 2.0f > pedPos.z) {
                    m_nSwimState = SWIM_UNDERWATER_SPRINTING;
                }
            }
        }
    }

    if (!m_bAnimBlockRefAdded) {
        CAnimBlock* animBlock = CAnimManager::GetAnimationBlock(ANIM_GROUP_SWIM);

        if (!animBlock) {
            const char* blockName = CAnimManager::GetAnimBlockName(ANIM_GROUP_SWIM);
            animBlock = CAnimManager::GetAnimationBlock(blockName);
        }

        const auto blockIndex = CAnimManager::GetAnimationBlockIndex(animBlock);
        if (animBlock->IsLoaded) {
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_bAnimBlockRefAdded = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
        }
    }

    if (!m_bFinishedBlending || !m_bAnimBlockRefAdded)
        return;

    switch (m_nSwimState) {
    case SWIM_TREAD: {
        if (m_AnimID == ANIM_ID_SWIM_TREAD)
            break;

        constexpr struct { AnimationId anim; float delta; } mapping[] = {
            { ANIM_ID_SWIM_BREAST,      -1.0f },
            { ANIM_ID_SWIM_CRAWL,       -1.0f },
            { ANIM_ID_SWIM_DIVE_UNDER,  -4.0f },
            { ANIM_ID_SWIM_UNDER,       -2.0f },
            { ANIM_ID_SWIM_GLIDE,       -2.0f },
            { ANIM_ID_SWIM_JUMPOUT,     -4.0f },
            { ANIM_ID_CLIMB_JUMP,       -4.0f },
        };
        for (const auto& [anim, delta] : mapping) {
            if (auto* assoc = RpAnimBlendClumpGetAssociation(player->GetRpClump(), anim)) {
                assoc->m_BlendDelta = delta;
            }
        }
        m_AnimID = ANIM_ID_SWIM_TREAD;
        break;
    }
    case SWIM_SPRINT: {
        if (m_AnimID == ANIM_ID_SWIM_BREAST) {
            CAnimBlendAssociation* assoc = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_BREAST);
            if (!assoc) {
                m_nSwimState = SWIM_TREAD;
                break;
            }
            if (player->IsPlayer()) {
                assoc->m_Speed = player->GetPlayerData()->m_fMoveBlendRatio;
            } else if (m_fAnimSpeed > 0.0f && m_fAnimSpeed <= 1.0f) {
                assoc->m_Speed = m_fAnimSpeed;
            }
        } else {
            CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_BREAST, 2.0f);
            m_AnimID = ANIM_ID_SWIM_BREAST;
        }
        break;
    }
    case SWIM_SPRINTING: {
        if (m_AnimID == ANIM_ID_SWIM_CRAWL) {
            CAnimBlendAssociation* assoc = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_CRAWL);
            if (!assoc) {
                m_nSwimState = SWIM_TREAD;
                break;
            }
            if (player->GetPlayerData()) {
                float buttonSprintResults = player->GetButtonSprintResults(SPRINT_WATER);
                assoc->m_Speed = std::max(1.0f, buttonSprintResults);
            } else if (m_fAnimSpeed > 1.0f) {
                assoc->m_Speed = std::min(1.5f, m_fAnimSpeed - 1.0f);
            }
        } else {
            CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_CRAWL, 2.0f);
            m_AnimID = ANIM_ID_SWIM_CRAWL;
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_AnimID == ANIM_ID_SWIM_DIVE_UNDER) {
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_DIVE_UNDER);
            if (!assocUnder) {
                m_nSwimState = SWIM_TREAD;
                break;
            }

            if (assocUnder->m_BlendHier->m_fTotalTime == assocUnder->m_CurrentTime) {
                m_nSwimState = SWIM_UNDERWATER_SPRINTING;
            }
        } else {
            CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_DIVE_UNDER, 8.0f);
            m_AnimID = ANIM_ID_SWIM_DIVE_UNDER;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        if ((m_AnimID == ANIM_ID_SWIM_UNDER || m_AnimID == ANIM_ID_SWIM_GLIDE) && m_fStateChanger >= 0.0f) {
            if (player->GetPlayerData() && player->GetButtonSprintResults(SPRINT_UNDERWATER) >= 1.0f) {
                CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_UNDER);
                if (!assocUnder || assocUnder->m_BlendDelta < 0.0f || assocUnder->m_BlendAmount == 0.0f) {
                    assocUnder = CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_UNDER, 4.0f);
                }
                if (assocUnder->m_BlendHier->m_fTotalTime == assocUnder->m_CurrentTime) {
                    assocUnder->Start(0.0f);
                    assocUnder->m_Speed = std::max(0.7f, player->GetButtonSprintResults(SPRINT_UNDERWATER));
                }
                m_AnimID = ANIM_ID_SWIM_UNDER;
            } else {
                CAnimBlendAssociation* assocGlide = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_GLIDE);
                if (!assocGlide || assocGlide->m_BlendDelta < 0.0f || assocGlide->m_BlendAmount == 0.0f) {
                    CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_GLIDE, 4.0f);
                }
                m_AnimID = ANIM_ID_SWIM_GLIDE;
            }
        } else {
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_UNDER);
            if (assocUnder) {
                if (m_fStateChanger < 0.0f && assocUnder->m_BlendAmount >= 0.99f) {
                    if (m_fStateChanger > -2.0f) {
                        // Como el original: ángulo global 0x8D2FC4 por 0x8595EC
                        m_fStateChanger = 0.0f;
                        m_fRotationX = SwimDiveUnderAngle * SWIM_DEG_TO_RAD;
                    } else {
                        m_fRotationX = SWIM_MAX_PITCH;
                        m_fStateChanger = 0.0f;
                    }
                }
            } else {
                CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_UNDER, 1000.0f);
                if (m_AnimID == ANIM_ID_SWIM_TREAD || m_AnimID == ANIM_ID_NO_ANIMATION_SET) {
                    m_fStateChanger = -2.0f;
                    m_AnimID = ANIM_ID_SWIM_UNDER;
                } else {
                    m_fStateChanger = -1.0f;
                    m_AnimID = ANIM_ID_SWIM_UNDER;
                }
            }
        }
        break;
    }
    case SWIM_BACK_TO_SURFACE: {
        if (m_AnimID == ANIM_ID_SWIM_JUMPOUT) {
            CAnimBlendAssociation* assocJumpOut = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_SWIM_JUMPOUT);
            if (!assocJumpOut) {
                m_nSwimState = SWIM_TREAD;
                break;
            }
            if (assocJumpOut->m_BlendHier->m_fTotalTime / 4.0f <= assocJumpOut->m_TimeStep + assocJumpOut->m_CurrentTime) {
                assocJumpOut = CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0f);
                assocJumpOut->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
                m_AnimID = ANIM_ID_CLIMB_JUMP;
            }
        } else if (m_AnimID == ANIM_ID_CLIMB_JUMP) {
            // Como el original: vuelve a flotar sin la animación o cuando la cabeza sigue 0,5 bajo el agua;
            // si no hay nivel de agua no cambia el estado (y siempre actualiza las estadísticas)
            if (!RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_CLIMB_JUMP)) {
                m_nSwimState = SWIM_TREAD;
            } else {
                float waterLevel1 = 0.0f;
                if (CWaterLevel::GetWaterLevel(pedPos, waterLevel1, true) && pedPos.z + 0.5f < waterLevel1) {
                    m_nSwimState = SWIM_TREAD;
                }
            }
        } else {
            CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_SWIM, ANIM_ID_SWIM_JUMPOUT, 8.0f);
            m_AnimID = ANIM_ID_SWIM_JUMPOUT;
            player->m_vecMoveSpeed.z = 8.0f / player->m_fMass;
            m_pEntity = CTaskSimpleClimb::TestForClimb(player, m_pClimbPos, m_fAngle, m_nSurfaceType, true);
            CEntity::SafeRegisterRef(m_pEntity);
        }
        break;
    }
    default: {
        break;
    }
    }

    if (player->GetPlayerData() && (player->GetPlayerData()->m_fMoveBlendRatio > 0.5f || m_nSwimState == SWIM_UNDERWATER_SPRINTING)) {
        CStats::UpdateStatsWhenSwimming(m_nSwimState == SWIM_UNDERWATER_SPRINTING, m_nSwimState == SWIM_SPRINTING);
    }
}

// 0x68A1D0
void CTaskSimpleSwim::ProcessSwimmingResistance(CPed* ped) {
    float fSubmergeZ = -1.0f;
    CVector vecPedMoveSpeed{};

    const auto AnimShiftSpeed = [ped](CVector& out) {
        const CMatrix& m = *ped->m_matrix;
        const float sx = ped->m_vecAnimMovingShiftLocal.x;
        const float sy = ped->m_vecAnimMovingShiftLocal.y;
        out.x = sy * m.GetForward().x + sx * m.GetRight().x;
        out.y = sy * m.GetForward().y + sx * m.GetRight().y;
        out.z = sy * m.GetForward().z + sx * m.GetRight().z;
    };

    switch (m_nSwimState) {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        float fAnimBlendSum = 0.0f;
        float fAnimBlendDifference = 1.0f;

        CAnimBlendAssociation* animSwimBreast = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_BREAST);
        CAnimBlendAssociation* animSwimCrawl = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_CRAWL);
        if (animSwimBreast) {
            fAnimBlendSum = 0.4f * animSwimBreast->m_BlendAmount;
            fAnimBlendDifference = 1.0f - animSwimBreast->m_BlendAmount;
        }
        if (animSwimCrawl) {
            fAnimBlendSum += 0.2f * animSwimCrawl->m_BlendAmount;
            fAnimBlendDifference -= animSwimCrawl->m_BlendAmount;
        }
        if (fAnimBlendDifference < 0.0f) {
            fAnimBlendDifference = 0.0f;
        }

        fSubmergeZ = fAnimBlendDifference * 0.55f + fAnimBlendSum;
        AnimShiftSpeed(vecPedMoveSpeed);
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        AnimShiftSpeed(vecPedMoveSpeed);

        auto animSwimDiveUnder = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_DIVE_UNDER);
        if (animSwimDiveUnder) {
            vecPedMoveSpeed.z = animSwimDiveUnder->m_CurrentTime / animSwimDiveUnder->m_BlendHier->m_fTotalTime * -0.1f;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        // Como el original: fcos/fsin completos; (cos·forward)·sy + sx·right y z = ((..) + sin·sy) + 0,01
        const CMatrix& m = *ped->m_matrix;
        const float sx = ped->m_vecAnimMovingShiftLocal.x;
        const float sy = ped->m_vecAnimMovingShiftLocal.y;
        float cs[4];
        CosSinProductsX87(m_fRotationX, &m.GetForward().x, sy, cs);
        vecPedMoveSpeed.x = cs[0] * sy + sx * m.GetRight().x;
        vecPedMoveSpeed.y = cs[1] * sy + sx * m.GetRight().y;
        vecPedMoveSpeed.z = cs[2] * sy + sx * m.GetRight().z + cs[3] + 0.01f;
        break;
    }
    case SWIM_BACK_TO_SURFACE: {
        auto animClimb = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_CLIMB_JUMP);
        if (!animClimb)
            animClimb = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SWIM_JUMPOUT);

        if (animClimb) {
            if (animClimb->m_BlendHier->m_fTotalTime > animClimb->m_CurrentTime &&
                (animClimb->m_BlendAmount >= 1.0f || animClimb->m_BlendDelta > 0.0f)
            ) {
                float fMoveForceZ = CTimer::GetTimeStep() * ped->m_fMass * 0.3f * 0.008f;
                ped->ApplyMoveForce(0.0f, 0.0f, fMoveForceZ);
            }
        }
        return;
    }
    default: // Como el original: un estado desconocido sigue con velocidad cero e inmersión -1
        break;
    }

    ApplySwimResistanceX87(CTimer::GetTimeStep(), &ped->m_vecMoveSpeed, &vecPedMoveSpeed);

    auto& pedPos = ped->GetPosition();
    const float ts = CTimer::GetTimeStep();
    // Como el original: solo x/y avanzan con la velocidad; la z es la de la posición
    float fWaterLevel = 0.0f;
    if (CWaterLevel::GetWaterLevel(ts * ped->m_vecMoveSpeed.x + pedPos.x, ts * ped->m_vecMoveSpeed.y + pedPos.y, pedPos.z, fWaterLevel, true, nullptr)) {
        if (m_nSwimState == SWIM_UNDERWATER_SPRINTING && !(m_fStateChanger < 0.0f)) {
            if (0.65f + pedPos.z > fWaterLevel && m_fRotationX > SWIM_QUARTER_PI) {
                m_nSwimState = SWIM_TREAD;
                m_fStateChanger = 0.0f;
            } else if (!(m_fRotationX < 0.0f)) {
                if (0.65f + pedPos.z > fWaterLevel) {
                    const float fMinimumSpeed = 0.05f * 0.5f;
                    if (m_fStateChanger > fMinimumSpeed) {
                        m_fStateChanger *= 0.95f;
                    }
                    if (m_fStateChanger < fMinimumSpeed) {
                        m_fStateChanger = std::min(fMinimumSpeed, ts * 0.002f + m_fStateChanger);
                    }
                    m_fRotationX = ts * m_fStateChanger + m_fRotationX;
                    fSubmergeZ = m_fRotationX * SWIM_FOUR_OVER_PI * (0.55f - 0.2f) * 0.75f + 0.2f;
                } else {
                    if (m_fStateChanger > 0.001f)
                        m_fStateChanger *= 0.95f;
                    else
                        m_fStateChanger = 0.0f;
                }
            } else {
                if (TrigCompoundSubSin(pedPos.z, m_fRotationX) + 0.65f > fWaterLevel) {
                    m_fStateChanger = ts * 0.002f + m_fStateChanger;
                    if (m_fStateChanger > 0.05f) {
                        m_fStateChanger = 0.05f;
                    }
                } else {
                    if (m_fStateChanger > 0.001f)
                        m_fStateChanger *= 0.95f;
                    else
                        m_fStateChanger = 0.0f;
                }
                m_fRotationX = ts * m_fStateChanger + m_fRotationX;
            }
        }

        if (fSubmergeZ > 0.0f) {
            fWaterLevel = fWaterLevel - (fSubmergeZ + pedPos.z);
            float fTimeStepMoveSpeedZ = fWaterLevel / ts;
            float fTimeStep = ts * 0.1f;
            if (fTimeStepMoveSpeedZ > fTimeStep) fTimeStepMoveSpeedZ = fTimeStep;
            if (-fTimeStep > fTimeStepMoveSpeedZ) fTimeStepMoveSpeedZ = -fTimeStep;
            fTimeStepMoveSpeedZ -= ped->m_vecMoveSpeed.z;
            fTimeStep = ts * 0.02f; // Como el original: 0x8708D8, no paso / 50
            if (fTimeStepMoveSpeedZ > fTimeStep) fTimeStepMoveSpeedZ = fTimeStep;
            if (-fTimeStep > fTimeStepMoveSpeedZ) fTimeStepMoveSpeedZ = -fTimeStep;
            ped->m_vecMoveSpeed.z = fTimeStepMoveSpeedZ + ped->m_vecMoveSpeed.z;
        }
    }

    if (pedPos.z < -69.0f) {
        pedPos.z = -69.0f;
        if (0.0f > ped->m_vecMoveSpeed.z) {
            ped->m_vecMoveSpeed.z = 0.0f;
        }
    }
}

// 0x68AA70
void CTaskSimpleSwim::ProcessEffects(CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    CVector particlePosition = ped->GetForwardVector();
    particlePosition *= 0.4f;
    particlePosition += pedPos;

    if (ped->GetPlayerData())
        particlePosition.z = ped->GetPlayerData()->m_fWaterHeight;
    else
        particlePosition.z += 0.5f;

    if (m_nSwimState != SWIM_TREAD) {
        DestroyFxSystem();
    } else {
        RwMatrix* mat = RwMatrixCreate();
        mat->pos = *((RwV3d*)&particlePosition);
        RwMatrixUpdate(mat);
        if (!m_pFxSystem) {
            CreateFxSystem(ped, mat);
        }

        if (m_pFxSystem) {
            m_pFxSystem->SetMatrix(mat);
            m_pFxSystem->SetLocalParticles(true);
        }
        RwMatrixDestroy(mat);
    }

    if (m_nSwimState != SWIM_DIVE_UNDERWATER)
        m_bTriggerWaterSplash = false;

    switch (m_nSwimState) {
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        const CVector& vecPedUp = ped->GetForward();

        float fRadianAngle = CGeneral::GetAngleBetweenPoints(vecPedUp.x, vecPedUp.y, 0.0f, 0.0f);
        float fLimitedRadianAngle = CGeneral::LimitAngle(fRadianAngle) + 180.0f;

        g_fx.m_Wake->AddParticle(particlePosition, {}, 0.0f, FxPrtMult_c(1.0f, 1.0f, 1.0f, 0.2f, 0.4f, 0.0f, 0.5f), fLimitedRadianAngle);
        ped->GetAE().AddAudioEvent(AE_PED_SWIM_WAKE, 0.0f, 1.0f);

        if (m_nSwimState == SWIM_SPRINTING) {
            RpHAnimHierarchy* animHierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump()); // todo: almost CPed::GetBoneMatrix
            const auto GetBonePos = [&](auto boneId) {
                auto index = RpHAnimIDGetIndex(animHierarchy, boneId);
                return &RpHAnimHierarchyGetMatrixArray(animHierarchy)[index].pos;
            };
            // Como el original: primero las cuatro posiciones y después las comprobaciones
            RwV3d* const bonePositions[] = { GetBonePos(BONE_R_HAND), GetBonePos(BONE_L_HAND), GetBonePos(BONE_R_FOOT), GetBonePos(BONE_L_FOOT) };
            const float pedZ = pedPos.z;
            for (RwV3d* bonePos : bonePositions) {
                if (std::fabs(bonePos->z - pedZ) < 0.05f) {
                    auto fx = g_fxMan.CreateFxSystem("water_swim", *bonePos, nullptr, false);
                    if (fx) {
                        fx->PlayAndKill();
                        ped->GetAE().AddAudioEvent(AE_PED_SWIM_STROKE_SPLASH, 0.0f, 1.0f);
                    }
                }
            }
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_bTriggerWaterSplash) {
            return;
        }
        g_fx.TriggerWaterSplash(particlePosition);
        m_bTriggerWaterSplash = true;
        ped->GetAE().AddAudioEvent(AE_PED_SWIM_DIVE_SPLASH, 0.0f, 1.0f);
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        // Como el original: entero con signo (_ftol2), por 1/3 (0x859040) y aleatorio de 0x407180
        int32 oxygen = 5;
        if (ped->IsPlayer()) {
            oxygen = static_cast<int32>((100.0f - ped->GetPlayerData()->m_fBreath / CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 100.0f) * SWIM_THIRD);
        }
        if (CGeneral::GetRandomIntInRange(0, 100) < oxygen) {
            RpHAnimHierarchy* hier = GetAnimHierarchyFromSkinClump(ped->GetRpClump()); // todo: almost CPed::GetBoneMatrix
            constexpr auto bubbleMatrix = 3; // Como el original: matriz 3 directamente (sin RpHAnimIDGetIndex)
            CVector* bonePos = static_cast<CVector*>(&RpHAnimHierarchyGetMatrixArray(hier)[bubbleMatrix].pos);

            static FxPrtMult_c fxPrtMult(1.0f, 1.0f, 1.0f, 0.25f, 0.3f, 0.0f, 0.5f);
            g_fx.m_Bubble->AddParticle(*bonePos, { 0.0f, 0.0f, 2.0f }, 0.0f, fxPrtMult);
        }
        break;
    }
    }
}

// 0x689640
void CTaskSimpleSwim::ProcessControlAI(CPed* ped) {
    m_nSwimState = SWIM_TREAD;
    if (m_pPed) {
        const CVector& pedPos     = ped->GetPosition();
        const CVector& taskPedPos = m_pPed->GetPosition();
        CVector vecPosition(taskPedPos.x - pedPos.x, taskPedPos.y - pedPos.y, taskPedPos.z - pedPos.z);
        vecPosition.Normalise();
        if (m_fRandomMoveBlendRatio == 0.0f) {
            m_fRandomMoveBlendRatio = float(CGeneral::GetRandomNumber() & 3) * 0.5f + 1.5f;
        }

        const float r = m_fRandomMoveBlendRatio;
        const CVector offset{ vecPosition.x * r, vecPosition.y * r, vecPosition.z * r };
        const CVector& leaderPos = m_pPed->GetPosition();
        m_vecPos = CVector{ leaderPos.x - offset.x, leaderPos.y - offset.y, leaderPos.z - offset.z };
    }

    const CVector& pedPos = ped->GetPosition();
    // Como el original: sqrt(dy·dy + dx·dx) recalculado en cada uso
    const auto Dist2D = [&] {
        const float dx = m_vecPos.x - pedPos.x;
        const float dy = m_vecPos.y - pedPos.y;
        return std::sqrt(dy * dy + dx * dx);
    };

    bool bPedGroupSet = false;
    if (m_vecPos.x != 0.0f || m_vecPos.y != 0.0f || m_vecPos.z != 0.0f) {
        ped->m_fAimingRotation = Atan2X87(-(m_vecPos.x - pedPos.x), m_vecPos.y - pedPos.y);

        const float fMinimum2DDistanceBetweenPeds = ped->IsPlayer() ? 0.5f : 1.0f;
        if (Dist2D() > fMinimum2DDistanceBetweenPeds) {
            m_nSwimState = SWIM_SPRINT;
            bool bCheckAnimSpeed = true;
            // Como el original: pide el grupo y la tarea de nadar dos veces; si no es el líder mira la velocidad
            if (m_pPed && CPedGroups::GetPedsGroup(ped) && CPedGroups::GetPedsGroup(ped)->GetMembership().GetLeader() == m_pPed) {
                bCheckAnimSpeed = false;
                bPedGroupSet = true;
                if (m_pPed->GetIntelligence()->GetTaskSwim() && m_pPed->GetIntelligence()->GetTaskSwim()->m_nSwimState == SWIM_SPRINTING) {
                    const CVector d = m_vecPos - pedPos;
                    if (std::sqrt(d.x * d.x + d.y * d.y) > 5.0f) { // Como el original: distancia 2D
                        m_nSwimState = SWIM_SPRINTING;
                    }
                }
            }
            if (bCheckAnimSpeed && m_fAnimSpeed > 1.0f && Dist2D() > 2.0f) {
                m_nSwimState = SWIM_SPRINTING;
            }
        }
    }

    // Como el original: bCanClimbOntoBoat (bit 13 de 0x478) y m_pPed leído sin comprobar en la segunda condición
    if (m_pPed && m_pPed->bIsStanding && !m_pPed->physicalFlags.bSubmergedInWater ||
        ped->bCanClimbOntoBoat && !m_pPed->physicalFlags.bSubmergedInWater && Dist2D() < 1.0f) {
        if (!((ped->m_nRandomSeedUpperByte + CTimer::GetFrameCounter() - 4) & 127)) {
            if (CTaskSimpleClimb::TestForClimb(ped, m_pClimbPos, m_fAngle, m_nSurfaceType, true)) {
                m_nSwimState = SWIM_BACK_TO_SURFACE;
            }
        }
    }
    m_fAimingRotation = 0.0f;
    m_fUpperTorsoRotationX = 0.0f;

    if (!bPedGroupSet && !ped->IsPlayer() && m_fAnimSpeed < 0.0f) {
        ped->m_vecAnimMovingShiftLocal.x *= SwimAnimShiftFactor;
        ped->m_vecAnimMovingShiftLocal.y *= SwimAnimShiftFactor;
    }
}

// 0x688A90
void CTaskSimpleSwim::ProcessControlInput(CPlayerPed* ped) {
    CPlayerPedData* playerData = ped->GetPlayerData();
    if (!m_bFinishedBlending || !m_bAnimBlockRefAdded) {
        playerData->m_fMoveBlendRatio = 0.0f;
        return;
    }

    CPad* pad = ped->GetPadFromPlayer();
    // Como el original: primero arriba/abajo y después izquierda/derecha
    const int16 walkUpDown = pad->GetPedWalkUpDown();
    float walkX = (float)pad->GetPedWalkLeftRight() * (1.0f / 128.0f);
    float walkY = (float)walkUpDown * (1.0f / 128.0f);
    float fWalkMagnitude = std::sqrt(walkY * walkY + walkX * walkX);
    if ((int16)m_nSwimState < SWIM_SPRINTING) {
        if (pad->JumpJustDown()) {
            m_nSwimState = SWIM_BACK_TO_SURFACE;
        } else if (pad->WeaponJustDown(ped)) {
            m_nSwimState = SWIM_DIVE_UNDERWATER;
            playerData->m_fMoveBlendRatio = 0.0f;
        }
    }

    const float ts = CTimer::GetTimeStep();
    switch ((int16)m_nSwimState) {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        float fRotation = walkX;
        bool bPlayerUse2PlayerControls = false;
        if (CGameLogic::IsPlayerUse2PlayerControls(ped)) {
            bPlayerUse2PlayerControls = true;
            if (fWalkMagnitude > 0.0f) {
                float angle = CGeneral::LimitRadianAngle(RadianAngleFromOriginSubX87(-walkX, walkY, TheCamera.m_fOrientation));
                // Como el original: (-sin, cos, 0) con fsin/fcos
                const CVector vecPedWalkDirection(-TrigSimpleSinCpp(angle), TrigSimpleCosCpp(angle), 0.0f);
                const float cur = ped->m_fCurrentRotation;
                if (angle > cur + SWIM_PI) {
                    angle -= SWIM_TWO_PI;
                } else if (angle < cur - SWIM_PI) {
                    angle += SWIM_TWO_PI;
                }

                const float fTurn = std::clamp(angle - cur, -1.0f, 1.0f);
                ped->m_fAimingRotation = WrapSwimAngle(fTurn * (ts * 0.08f) + cur);
                if (CGameLogic::IsPlayerAllowedToGoInThisDirection(ped, vecPedWalkDirection, 0.0f)) {
                    const CMatrix& m = *ped->m_matrix;
                    fRotation = (vecPedWalkDirection.y * m.GetRight().y + vecPedWalkDirection.x * m.GetRight().x + m.GetRight().z * 0.0f) * fWalkMagnitude;
                    walkY = -((vecPedWalkDirection.y * m.GetForward().y + m.GetForward().z * 0.0f + vecPedWalkDirection.x * m.GetForward().x) * fWalkMagnitude);
                } else {
                    fRotation = 0.0f;
                    fWalkMagnitude = 0.0f;
                    walkY = 0.0f;
                }
            }
        }

        if (CCamera::m_bUseMouse3rdPerson) {
            const auto& camFront = CCamera::GetActiveCamera().m_vecFront;
            ped->m_fAimingRotation = Atan2X87(-camFront.x, camFront.y); // heading
            if (TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD) {
                ped->m_fAimingRotation += SWIM_PI;
                if (ped->m_fAimingRotation > SWIM_PI) {
                    ped->m_fAimingRotation -= SWIM_TWO_PI;
                }
            }

            // 0x6891C9
            const float negativePedWalkY = -walkY;
            const float fTimeStep = ts * 0.07f;
            if (negativePedWalkY - playerData->m_fMoveBlendRatio > fTimeStep) {
                playerData->m_fMoveBlendRatio = fTimeStep + playerData->m_fMoveBlendRatio;
            } else {
                playerData->m_fMoveBlendRatio = negativePedWalkY;
            }

            // Como el original: por debajo de -pi suma 2pi
            fRotation = WrapSwimAngle(-(ped->m_fAimingRotation - ped->m_fCurrentRotation));
            fRotation = std::clamp(fRotation * 10.0f, -1.0f, 1.0f);
        } else if (fWalkMagnitude > 0.0f) {
            if (!bPlayerUse2PlayerControls) {
                ped->m_fAimingRotation = WrapSwimAngle(ts * -0.03f * fRotation + ped->m_fAimingRotation);
                if (m_nSwimState == SWIM_TREAD) {
                    fWalkMagnitude = -walkY;
                } else if (walkY > 0.0f) {
                    fWalkMagnitude -= walkY;
                }
                if (fWalkMagnitude < 0.0f) {
                    fWalkMagnitude = 0.0f;
                }
            }

            const float fTimeStep = ts * 0.07f;
            if (fWalkMagnitude - playerData->m_fMoveBlendRatio > fTimeStep) {
                playerData->m_fMoveBlendRatio = fTimeStep + playerData->m_fMoveBlendRatio;
            } else {
                playerData->m_fMoveBlendRatio = fWalkMagnitude;
            }
        } else if (playerData->m_fMoveBlendRatio > 0.0f) {
            const float mbr = playerData->m_fMoveBlendRatio - ts * 0.05f;
            playerData->m_fMoveBlendRatio = 0.0f > mbr ? 0.0f : mbr;
        }

        m_fAimingRotation += ts * 0.08f * fRotation;
        if (m_nSwimState == SWIM_SPRINTING) {
            m_fTurningRotationY += ts * 0.04f * fRotation;
        } else if (m_nSwimState == SWIM_SPRINT) {
            m_fUpperTorsoRotationX += std::fabs(fRotation) * ts * 0.04f; // Como el original: el giro (no el eje X del mando)
        }

        if (m_nSwimState == SWIM_SPRINTING) {
            float fWaterLevel1 = 0.0f, fWaterLevel2 = 0.0f;
            const auto& pedPos = ped->GetPosition();
            const auto& pedFwd = ped->m_matrix->GetForward();
            const CVector position = pedPos + pedFwd;
            const CVector distance = pedPos - pedFwd;
            if (CWaterLevel::GetWaterLevel(position.x, position.y, position.z, fWaterLevel1, true, nullptr) &&
                CWaterLevel::GetWaterLevel(distance.x, distance.y, distance.z, fWaterLevel2, true, nullptr)) {
                m_fRotationX = Atan2X87(fWaterLevel1 - fWaterLevel2, 2.0f) * 1.0f;
            }
        }

        if (!(ped->ControlButtonSprint(SPRINT_WATER) >= 1.0f)) {
            m_nSwimState = playerData->m_fMoveBlendRatio > 0.5f ? SWIM_SPRINT : SWIM_TREAD;
        } else {
            m_nSwimState = SWIM_SPRINTING;
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_fStateChanger > 0.0f) {
            m_fStateChanger = 0.0f;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        if (CCamera::m_bUseMouse3rdPerson) {
            CVector vecActiveCamFront = CCamera::GetActiveCamera().m_vecFront;
            if (TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD) {
                vecActiveCamFront.x *= -1.0f;
                vecActiveCamFront.z = 0.0f;
                vecActiveCamFront.y *= -1.0f;
            }

            // Como el original: la resta usa el fpatan completo (sin redondear antes a float)
            float aimingRotation;
            float fRotation = -Atan2SubX87(-vecActiveCamFront.x, vecActiveCamFront.y, ped->m_fCurrentRotation, &aimingRotation);
            ped->m_fAimingRotation = aimingRotation;
            fRotation = WrapSwimAngle(fRotation);
            fRotation = std::clamp(fRotation * 10.0f, -1.0f, 1.0f);

            m_fTurningRotationY += ts * 0.04f * fRotation;
            m_fAimingRotation   += ts * 0.08f * fRotation;

            const float fRotationX = std::clamp(AsinSubX87(vecActiveCamFront.z, m_fRotationX) * 10.0f, -1.0f, 1.0f);
            if (m_fStateChanger == 0.0f || fRotationX > 0.0f) {
                m_fRotationX += ts * 0.02f * fRotationX; // Como el original: 0x870884, no paso / 50
            }
            const float rotX = std::clamp(m_fTurningRotationY / 0.5f, -1.0f, 1.0f);
            m_fUpperTorsoRotationX += ts * -0.08f * std::clamp(fRotationX + rotX * -0.08f * fRotation, -1.0f, 1.0f);
        } else if (fWalkMagnitude > 0.0f) {
            const float fNormalizedWalkMagnitude = 1.0f / fWalkMagnitude;
            const float pedWalkX = walkX * fNormalizedWalkMagnitude;
            const float pedWalkY = fNormalizedWalkMagnitude * walkY;
            ped->m_fAimingRotation = WrapSwimAngle(ts * -0.03f * pedWalkX + ped->m_fAimingRotation);

            m_fTurningRotationY += ts * 0.04f * pedWalkX;
            m_fAimingRotation   += ts * 0.08f * pedWalkX;
            if (m_fStateChanger == 0.0f || pedWalkY > 0.0f) {
                m_fRotationX += ts * 0.02f * pedWalkY;
            }
            const float rotX = std::clamp(m_fTurningRotationY / 0.5f, -1.0f, 1.0f);
            m_fUpperTorsoRotationX += ts * -0.08f * std::clamp(pedWalkY + rotX * -0.08f * pedWalkX, -1.0f, 1.0f);
        }
        m_fRotationX = std::clamp(ts * 0.001f + m_fRotationX, -SWIM_MAX_PITCH, SWIM_MAX_PITCH); // Como el original: 0x8708B4
        // BUG: it should be m_fTimeCanRun <= 0.1f
        if (playerData->m_fTimeCanRun <= 0.0f) {
            playerData->m_fTimeCanRun = 0.1f;
        }
        ped->ControlButtonSprint(SPRINT_UNDERWATER);
        break;
    }
    }

    // Como el original: pow del EXE con el valor completo de ST0 en los productos
    const int16 state = (int16)m_nSwimState;
    float fRotation = 0.95f; // 0x8708A8 / 0x8708B0, iguales

    if (m_fTurningRotationY > 0.01f || m_fTurningRotationY < -0.01f) {
        m_fTurningRotationY = MulPowX87(fRotation, ts, m_fTurningRotationY);
    } else {
        m_fTurningRotationY = 0.0f;
    }

    if (state != SWIM_UNDERWATER_SPRINTING && state != SWIM_SPRINTING) {
        if (m_fRotationX > 0.01f || m_fRotationX < -0.01f) {
            m_fRotationX = MulPowX87(fRotation, ts, m_fRotationX);
        } else {
            m_fRotationX = 0.0f;
        }
    }

    fRotation = state == SWIM_DIVE_UNDERWATER || state == SWIM_BACK_TO_SURFACE ? 0.95f : 0.92f;

    if (m_fAimingRotation > 0.01f || m_fAimingRotation < -0.01f || m_fUpperTorsoRotationX > 0.01f || m_fUpperTorsoRotationX < -0.01f) {
        m_fAimingRotation      = MulPowX87(fRotation, ts, m_fAimingRotation);
        m_fUpperTorsoRotationX = MulPowX87(fRotation, ts, m_fUpperTorsoRotationX);
    } else {
        m_fAimingRotation = 0.0f;
        m_fUpperTorsoRotationX = 0.0f;
    }

    if (state == SWIM_SPRINT)
        ped->HandleSprintEnergy(false, 0.5f);
    else if (state != SWIM_SPRINTING)
        ped->HandleSprintEnergy(false, 1.0f);
}
