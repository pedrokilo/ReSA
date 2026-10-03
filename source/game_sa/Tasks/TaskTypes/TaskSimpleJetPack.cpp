/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleJetPack
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleJetPack.h"
#include "TaskSimplePlayerOnFoot.h"

constexpr CVector JETPACK_POS_OFFSET   = { 0.1f, 0.08f, 0.0f };
constexpr CVector JETPACK_ROT_AXIS     = { 0.0f, 1.0f, 0.0f };
constexpr CVector JETBALL_OFFSET       = { 0.f, 0.f, -0.15f };

constexpr float MAGIC = 0.008f; // == 1 / 125

constexpr float THRUST_NOMINAL         = 0.8f;
constexpr float THRUST_FULL            = 0.6f;
constexpr float THRUST_STRAFE          = 0.3f;
constexpr float THRUST_STOP            = 0.5f;
constexpr float THRUST_MAX_ANGLE       = std::bit_cast<float>(0x3FA78D36u); // 0x8D2F48 - Como el original: 75º en radianes (no 1.309f)
constexpr float THRUST_MOVE_DAMPING    = 0.02f;

constexpr float JETPACK_TURN_RATE      = -0.05f;
constexpr float JETPACK_ANGLE_RATE     = 0.9f;

constexpr float LEG_SWING_MAX_ANGLE    = std::bit_cast<float>(0x3F490FDBu); // 0x8D2F58 - Como el original: pi/4 exacto (no 0.7854f)
constexpr float LEG_SWING_DELTA_V_MULT = -0.2f;
constexpr float LEG_SWING_GRAVITY_MULT = 0.01f;
constexpr float LEG_SWING_DAMP_FRAC    = 0.98f;

namespace {
// Como el original: constantes exactas del EXE
const float kJpRadToDeg      = std::bit_cast<float>(0x42652EE0u); // 0x859878 (no es el float más cercano a 180/pi)
const float kJpPi            = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
const float kJpTwoPi         = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float kJpMinusPi       = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
const float kJpInv60         = std::bit_cast<float>(0x3C888889u); // 0x859044 (se multiplica, no se divide entre 60)
const float kJpTimeToRad     = std::bit_cast<float>(0x3ACDE32Eu); // 0x870640
const float kJpJetBallPeriod = std::bit_cast<float>(0x429F27AAu); // 0x8D2F80
const float kJpRandRecip     = std::bit_cast<float>(0x38000100u); // 0x858C7C (1/32767)
const float kJpTwo32         = 4294967296.0f;                     // 0x858C54
const float kJpOne           = 1.0f;
const float kJpZero          = 0.0f;
const float kJpThrustMaxAngle = THRUST_MAX_ANGLE;
const float kJpAngleRate     = JETPACK_ANGLE_RATE;
const float kJpSwingDamp     = LEG_SWING_DAMP_FRAC;
const float kJpSwingDeltaV   = LEG_SWING_DELTA_V_MULT;
const float kJpSwingGravity  = LEG_SWING_GRAVITY_MULT;
const float kJpSwingMax      = LEG_SWING_MAX_ANGLE;
const float kJpMoveDamping   = THRUST_MOVE_DAMPING;

// atan2(y, x) con fpatan (precisión completa, se redondea al guardar)
float __declspec(naked) __cdecl JpAtan2X87(float y, float x) {
    __asm {
        fld   dword ptr [esp+4]
        fld   dword ptr [esp+8]
        fpatan
        ret
    }
}

// (float)v · k con v sin signo (fild + 2^32 si es negativo) y un solo redondeo
float __declspec(naked) __cdecl JpUIntMulX87(uint32 v, float k) {
    __asm {
        mov   eax, dword ptr [esp+4]
        fild  dword ptr [esp+4]
        test  eax, eax
        jge   L_pos
        fadd  dword ptr [kJpTwo32]
    L_pos:
        fmul  dword ptr [esp+8]
        ret
    }
}

// (float)v / k con v sin signo (fild + 2^32 si es negativo) y un solo redondeo
float __declspec(naked) __cdecl JpUIntDivX87(uint32 v, float k) {
    __asm {
        mov   eax, dword ptr [esp+4]
        fild  dword ptr [esp+4]
        test  eax, eax
        jge   L_pos
        fadd  dword ptr [kJpTwo32]
    L_pos:
        fdiv  dword ptr [esp+8]
        ret
    }
}

// fsin(a) · k con el seno sin redondear
float __declspec(naked) __cdecl JpMulSinX87(float a, float k) {
    __asm {
        fld   dword ptr [esp+4]
        fsin
        fmul  dword ptr [esp+8]
        ret
    }
}

// fcos(a) · k con el coseno sin redondear
float __declspec(naked) __cdecl JpMulCosX87(float a, float k) {
    __asm {
        fld   dword ptr [esp+4]
        fcos
        fmul  dword ptr [esp+8]
        ret
    }
}

// (1 - p) · udS · ángulo máximo + p · angle con p = pow(0.9, paso) del _CIpow del EXE (0x822130) en ST0 completo
float __declspec(naked) __cdecl JpLerpThrustAngleX87(float udS, float angle, float step) {
    __asm {
        fld   dword ptr [kJpAngleRate]
        fld   dword ptr [esp+0Ch]
        mov   eax, 0x822130
        call  eax
        fld   dword ptr [kJpOne]
        fsub  st, st(1)
        fmul  dword ptr [esp+4]
        fmul  dword ptr [kJpThrustMaxAngle]
        fxch  st(1)
        fmul  dword ptr [esp+8]
        faddp st(1), st
        ret
    }
}

// Como el original (0x67F1C2): cos/sin del ángulo de empuje sin redondear por las componentes de arriba y delante;
// out[0..2] = delante · -sin + arriba · cos (out[3..7] temporales)
void JpThrustDirX87(float thrustAngle, const float* upVec, const float* fwdVec, float* result) {
    __asm {
        mov   eax, upVec
        mov   edx, fwdVec
        mov   ecx, result
        fld   thrustAngle
        fcos
        fld   st(0)
        fmul  dword ptr [eax]
        fld   st(1)
        fmul  dword ptr [eax+4]
        fstp  dword ptr [ecx+0Ch]
        fxch  st(1)
        fmul  dword ptr [eax+8]
        fstp  dword ptr [ecx+10h]
        fld   thrustAngle
        fsin
        fchs
        fld   st(0)
        fmul  dword ptr [edx]
        fstp  dword ptr [ecx+14h]
        fld   st(0)
        fmul  dword ptr [edx+4]
        fstp  dword ptr [ecx+18h]
        fmul  dword ptr [edx+8]
        fstp  dword ptr [ecx+1Ch]
        fld   dword ptr [ecx+14h]
        fadd  st, st(1)
        fstp  dword ptr [ecx]
        fstp  st(0)
        fld   dword ptr [ecx+18h]
        fadd  dword ptr [ecx+0Ch]
        fstp  dword ptr [ecx+4]
        fld   dword ptr [ecx+1Ch]
        fadd  dword ptr [ecx+10h]
        fstp  dword ptr [ecx+8]
    }
}

// Como el original (0x67F43D..0x67F660): balanceo de piernas y amortiguación de la velocidad transcritos del x87.
// damp = pow(0.98, paso) queda en ST0 completo para las dos velocidades y el giro; v *= pow(1 - |v| · 0.02, paso).
// task: CTaskSimpleJetPack (0x18/0x1C balanceo, 0x20 giro, 0x24/0x28 velocidades, 0x30 velocidad previa)
void JpSwingTailX87(void* task, CPed* ped, const CVector* prevPedVel, float step) {
    float accZ, deltaY, deltaZ, sumX, sumY, sumZ, angTmp;
    __asm {
        mov   ebx, task
        mov   edi, ped
        lea   esi, [edi+44h]
        mov   ecx, prevPedVel
        fld   dword ptr [esi]
        fsub  dword ptr [ecx]
        fld   dword ptr [esi+4]
        fsub  dword ptr [ecx+4]
        fld   dword ptr [esi+8]
        fsub  dword ptr [ecx+8]
        fstp  accZ
        fld   dword ptr [esi]
        fsub  dword ptr [ebx+30h]
        fld   dword ptr [esi+4]
        fsub  dword ptr [ebx+34h]
        fstp  deltaY
        fld   dword ptr [esi+8]
        fsub  dword ptr [ebx+38h]
        fstp  deltaZ
        fadd  st, st(2)
        fstp  sumX
        fld   deltaY
        fadd  st, st(1)
        fstp  sumY
        fstp  st(0)
        fstp  st(0)
        fld   deltaZ
        fadd  accZ
        fstp  sumZ
        fld   dword ptr [kJpSwingDamp]
        fld   step
        mov   eax, 0x822130
        call  eax
        // hacia delante (matriz + 0x10)
        mov   eax, dword ptr [edi+14h]
        fld   sumZ
        fmul  dword ptr [eax+18h]
        fld   sumY
        fmul  dword ptr [eax+14h]
        faddp st(1), st
        fld   sumX
        fmul  dword ptr [eax+10h]
        faddp st(1), st
        test  byte ptr [edi+46Ch], 3
        je    L_fwdAir
        fstp  st(0)
        fld   dword ptr [kJpZero]
    L_fwdAir:
        fmul  dword ptr [kJpSwingDeltaV]
        fsubr dword ptr [ebx+24h]
        fst   dword ptr [ebx+24h]
        fld   dword ptr [ebx+18h]
        fsin
        fmul  step
        fmul  dword ptr [kJpSwingGravity]
        fsubp st(1), st
        fmul  st, st(1)
        fst   dword ptr [ebx+24h]
        fmul  step
        fadd  dword ptr [ebx+18h]
        fst   angTmp
        fst   dword ptr [ebx+18h]
        fcomp dword ptr [kJpSwingMax]
        fnstsw ax
        test  ah, 41h
        jne   L_fwdNotMax
        mov   eax, dword ptr [kJpSwingMax]
        mov   dword ptr [ebx+18h], eax
        jmp   L_fwdDone
    L_fwdNotMax:
        fld   dword ptr [kJpSwingMax]
        fchs
        fld   angTmp
        fcomp st(1)
        fnstsw ax
        test  ah, 5
        jp    L_fwdKeep
        fstp  dword ptr [ebx+18h]
        jmp   L_fwdDone
    L_fwdKeep:
        fstp  st(0)
    L_fwdDone:
        // lateral (matriz + 0x00)
        mov   eax, dword ptr [edi+14h]
        fld   sumZ
        fmul  dword ptr [eax+8]
        fld   sumY
        fmul  dword ptr [eax+4]
        faddp st(1), st
        fld   sumX
        fmul  dword ptr [eax]
        faddp st(1), st
        test  byte ptr [edi+46Ch], 3
        je    L_sideAir
        fstp  st(0)
        fld   dword ptr [kJpZero]
    L_sideAir:
        fmul  dword ptr [kJpSwingDeltaV]
        fsubr dword ptr [ebx+28h]
        fst   dword ptr [ebx+28h]
        fld   dword ptr [ebx+1Ch]
        fsin
        fmul  step
        fmul  dword ptr [kJpSwingGravity]
        fsubp st(1), st
        fmul  st, st(1)
        fst   dword ptr [ebx+28h]
        fmul  step
        fadd  dword ptr [ebx+1Ch]
        fst   angTmp
        fst   dword ptr [ebx+1Ch]
        fcomp dword ptr [kJpSwingMax]
        fnstsw ax
        test  ah, 41h
        jne   L_sideNotMax
        mov   eax, dword ptr [kJpSwingMax]
        mov   dword ptr [ebx+1Ch], eax
        jmp   L_sideDone
    L_sideNotMax:
        fld   dword ptr [kJpSwingMax]
        fchs
        fld   angTmp
        fcomp st(1)
        fnstsw ax
        test  ah, 5
        jp    L_sideKeep
        fstp  dword ptr [ebx+1Ch]
        jmp   L_sideDone
    L_sideKeep:
        fstp  st(0)
    L_sideDone:
        // giro: damp · giro
        fmul  dword ptr [ebx+20h]
        fstp  dword ptr [ebx+20h]
        // |v| = sqrt((x² + y²) + z²)
        fld   dword ptr [esi+8]
        fld   dword ptr [esi+4]
        fld   dword ptr [esi]
        fld   st(0)
        fmul  st, st(1)
        fld   st(2)
        fmul  st, st(3)
        faddp st(1), st
        fld   st(3)
        fmul  st, st(4)
        faddp st(1), st
        fsqrt
        fstp  angTmp
        fstp  st(0)
        fstp  st(0)
        fstp  st(0)
        fld   angTmp
        fmul  dword ptr [kJpMoveDamping]
        fsubr dword ptr [kJpOne]
        fld   step
        mov   eax, 0x822130
        call  eax
        fld   st(0)
        fmul  dword ptr [esi]
        fstp  sumX
        fld   st(0)
        fmul  dword ptr [esi+4]
        fstp  sumY
        fmul  dword ptr [esi+8]
        fstp  sumZ
        mov   eax, sumX
        mov   ecx, sumY
        mov   edx, sumZ
        mov   dword ptr [esi], eax
        mov   dword ptr [esi+4], ecx
        mov   dword ptr [esi+8], edx
        mov   dword ptr [ebx+30h], eax
        mov   dword ptr [ebx+34h], ecx
        mov   dword ptr [ebx+38h], edx
    }
}
} // namespace

void CTaskSimpleJetPack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleJetPack, 0x8705c4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x67B4E0);
    RH_ScopedInstall(Destructor, 0x67E600);

    RH_ScopedInstall(RenderJetPack, 0x67F6A0);

    RH_ScopedInstall(ProcessAnims, 0x67B5C0);
    RH_ScopedInstall(DropJetPack, 0x67B660);
    RH_ScopedInstall(ApplyRollAndPitch, 0x67B720);
    RH_ScopedInstall(DoJetPackEffect, 0x67B7F0);
    RH_ScopedInstall(StopJetPackEffect, 0x67BA10);
    RH_ScopedInstall(ProcessControlInput, 0x67E7B0);
    RH_ScopedInstall(ProcessThrust, 0x67EF20);

    RH_ScopedVMTInstall(Clone, 0x67C690);
    RH_ScopedVMTInstall(GetTaskType, 0x67B5B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67E6E0);
    RH_ScopedVMTInstall(ProcessPed, 0x6801F0);
}

// 0x67B4E0
CTaskSimpleJetPack::CTaskSimpleJetPack(const CVector* targetPos, float cruiseHeight, int32 hoverTime, CEntity* targetEntity) :
    m_HoverTime{ hoverTime },
    m_CruiseHeight{ cruiseHeight },
    m_TargetEntity{ targetEntity }
{
    CEntity::SafeRegisterRef(m_TargetEntity);

    if (targetPos)
        m_TargetPos = *targetPos;
}

CTaskSimpleJetPack::CTaskSimpleJetPack(const CTaskSimpleJetPack& o) :
    CTaskSimpleJetPack{ &o.m_TargetPos, o.m_CruiseHeight, o.m_HoverTime, o.m_TargetEntity }
{
}

// 0x67E600
CTaskSimpleJetPack::~CTaskSimpleJetPack() {
    if (m_JetPackClump) {
        CVisibilityPlugins::GetClumpModelInfo(m_JetPackClump)->RemoveRef();
        if (const auto a = GetFirstAtomic(m_JetPackClump)) {
            if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(a))) {
                RpClumpForAllAtomics(m_JetPackClump, AtomicRemoveAnimFromSkinCB, nullptr);
            }
        }
        RpClumpDestroy(m_JetPackClump);
        m_JetPackClump = nullptr; // Como el original: el EXE pone el puntero a nulo
    }
    StopJetPackEffect();
    CEntity::SafeCleanUpRef(m_TargetEntity);
}

// 0x67E6E0
bool CTaskSimpleJetPack::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (event) {
        if (event->GetEventType() == EVENT_DAMAGE) {
            if (ped->m_fHealth > 0.f || (!ped->bIsStanding && !ped->physicalFlags.bSubmergedInWater)) {
                return false;
            }
        }

        if (ped->IsPlayer() && event->GetEventType() == EVENT_ON_FIRE) {
            return true;
        }

        if (event->GetEventPriority() < 63) {
            return false;
        }
    }

    if (m_bSwitchedWeapons && ped->IsPlayer()) {
        ped->ReplaceWeaponWhenExitingVehicle();
        m_bSwitchedWeapons = false;
    }

    m_bIsFinished = true;

    StopJetPackEffect();
    ped->GetAE().TurnOffJetPack();

    return true;
}

// 0x6801F0
bool CTaskSimpleJetPack::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        if (m_bSwitchedWeapons && ped->IsPlayer()) {
            ped->ReplaceWeaponWhenExitingVehicle();
        }
        StopJetPackEffect();
        return true;
    }

    if (!m_bSwitchedWeapons) {
        if (ped->IsPlayer()) {
            ped->RemoveWeaponWhenEnteringVehicle(true);
        }
        m_bSwitchedWeapons = true; // Como el original: también si no es jugador
    }

    ped->bCheckColAboveHead = true;

    if (ProcessAnims(ped)) {
        if (ped->m_fHealth < 1.f) { // Como el original: con NaN se controla como jugador
            m_ThrustStop   = false;
            m_ThrustFwd    = 0;
            m_ThrustAngle  = 0.f;
            // Como el original: rand() & 1 suma 1 y si no resta 1 (antes la precedencia lo dejaba en +-1)
            m_ThrustStrafe = (CGeneral::GetRandomNumber() & 1) ? m_ThrustStrafe + 1.f : m_ThrustStrafe - 1.f;
            if (m_ThrustStrafe > 3.f) {
                m_ThrustStrafe = 3.f;
            } else if (m_ThrustStrafe < -3.f) {
                m_ThrustStrafe = -3.f;
            }

            // Como el original: t = ms · 0.0015707964 (un redondeo); ((fsin(t) · paso) · -0.05) · rand() · (1/32767)
            const float t  = JpUIntMulX87(CTimer::m_snTimeInMilliseconds, kJpTimeToRad);
            const float rf = (float)CGeneral::GetRandomNumber() * kJpRandRecip;
            ped->m_fAimingRotation = JpMulSinX87(t, CTimer::GetTimeStep()) * JETPACK_TURN_RATE * rf + ped->m_fAimingRotation;

            if (ped->GetPlayerData()) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 0.f;
            }
        } else if (ped->IsPlayer()) {
            ProcessControlInput(ped->AsPlayer());
        }
        ProcessThrust(ped);
        if (m_ThrustFwd > 0 || std::abs(m_ThrustStrafe) > 0.1f) {
            ped->bIsStanding = false;
        }
    }

    return false;
}

// 0x67F6A0
void CTaskSimpleJetPack::RenderJetPack(CPed* ped) {
    if (m_JetPackClump && !m_bIsFinished) {
        RwMatrix* const boneMats = RpHAnimHierarchyGetMatrixArray(GetAnimHierarchyFromSkinClump(ped->GetRpClump()));
        const auto jp = RpClumpGetFrame(m_JetPackClump);

        // Update JetPack matrix
        {
            const auto jpMat = RwFrameGetMatrix(jp);
            std::memcpy(jpMat, &boneMats[PED_NODE_LEFT_ARM], sizeof(RwMatrix)); // Como el original: rep movs de la matriz entera
            RwMatrixTranslate(jpMat, &JETPACK_POS_OFFSET, rwCOMBINEPRECONCAT);
            RwMatrixRotate(jpMat, &JETPACK_ROT_AXIS, 90.f, rwCOMBINEPRECONCAT);
        }

        // Como el original: comparaciones estrictas con pi exacto del EXE
        auto currHeading = ped->m_fAimingRotation;
        if (currHeading > m_PrevHeading + kJpPi) {
            currHeading -= kJpTwoPi;
        } else if (currHeading < m_PrevHeading - kJpPi) {
            currHeading += kJpTwoPi;
        }
        auto headingDelta = currHeading - m_PrevHeading;
        if (headingDelta > 0.2f) {
            headingDelta = 0.2f;
        } else if (headingDelta < -0.2f) {
            headingDelta = -0.2f;
        }
        // Como el original: (delta · 180/pi) · 10, que se suma en un lado y se resta en el otro
        const float headingDeg = headingDelta * kJpRadToDeg * 10.f;

        const auto ProcessJetBall = [&](const char* jbFrameName, bool bLeftSide) {
            const auto jb = CClumpModelInfo::GetFrameFromName(m_JetPackClump, jbFrameName);
            if (!jb) {
                return;
            }
            float x, y;
            if (m_ThrustStop) {
                const float t = JpUIntDivX87(CTimer::m_snTimeInMilliseconds, kJpJetBallPeriod);
                x = JpMulSinX87(t, bLeftSide ? 45.f : -45.f);
                y = JpMulCosX87(t, 45.f);
            } else {
                const float a = bLeftSide ? m_ThrustAngle * kJpRadToDeg + headingDeg : m_ThrustAngle * kJpRadToDeg - headingDeg;
                x = 90.f < a ? 90.f : (-90.f > a ? -90.f : a);
                const float rotY = m_ThrustStrafe * 45.f;
                y = bLeftSide ? (0.f > rotY ? 0.f : rotY) : (0.f < rotY ? 0.f : rotY);
            }
            const auto  jbMat = RwFrameGetMatrix(jb);
            const RwV3d jbPos = *RwMatrixGetPos(jbMat);
            RwMatrixRotate(jbMat, &CPedIK::XaxisIK, x, rwCOMBINEREPLACE);
            RwMatrixRotate(jbMat, &CPedIK::YaxisIK, y, rwCOMBINEPRECONCAT);
            RwV3dAssign(RwMatrixGetPos(jbMat), &jbPos); // Changing the rotation also changes the position I assume, so restore it
        };

        ProcessJetBall("jetball1", true);
        ProcessJetBall("jetball2", false);

        RwFrameUpdateObjects(jp);
        RpClumpRender(m_JetPackClump);
        DoJetPackEffect(ped);
    } else {
        StopJetPackEffect();
    }
    m_PrevHeading = ped->m_fCurrentRotation;
}

// 0x67EF20
void CTaskSimpleJetPack::ProcessThrust(CPed* ped) {
    // Como el original: fuerzas ((dir · 0.008) · mult) · masa · paso con la matriz leída sin reservarla
    CVector& speed = ped->m_vecMoveSpeed;
    const CVector prevPedVel = speed;
    const float step = CTimer::GetTimeStep();

    if (m_ThrustStop) {
        const CVector& up = ped->m_matrix->GetUp();
        const float mass = ped->m_fMass;
        ped->ApplyMoveForce(CVector{ up.x * MAGIC * mass * step, up.y * MAGIC * mass * step, up.z * MAGIC * mass * step });

        const float pedSpdSq = speed.x * speed.x + speed.y * speed.y + speed.z * speed.z;
        auto stopForce = step * THRUST_STOP * MAGIC;
        if (stopForce * stopForce > pedSpdSq) {
            stopForce *= 0.75f;
        }
        if (stopForce * stopForce > pedSpdSq) {
            speed = CVector{ 0.f, 0.f, 0.f };
        } else {
            // Como el original: (-v · (1 / |v|)) · fuerza · masa, sin paso
            const float nx = speed.x * -1.f, ny = speed.y * -1.f, nz = speed.z * -1.f;
            const float inv = 1.f / std::sqrt(pedSpdSq);
            const float m2 = ped->m_fMass;
            ped->ApplyMoveForce(CVector{ nx * inv * stopForce * m2, ny * inv * stopForce * m2, nz * inv * stopForce * m2 });
        }
    } else {
        float nominal = THRUST_NOMINAL;
        if (m_ThrustFwd < 0) {
            nominal *= 0.5f;
        }
        {
            const CVector& up = ped->m_matrix->GetUp();
            const float mass = ped->m_fMass;
            ped->ApplyMoveForce(CVector{ up.x * MAGIC * nominal * mass * step, up.y * MAGIC * nominal * mass * step, up.z * MAGIC * nominal * mass * step });
        }

        if (m_ThrustFwd > 0) { // 0x67F1B7
            const CMatrix& mat = *ped->m_matrix;
            float t[8];
            JpThrustDirX87(m_ThrustAngle, &mat.GetUp().x, &mat.GetForward().x, t);
            if (t[2] > 0.75f) { // Como el original: se limita la z de la suma (no la de delante)
                t[2] = 0.75f;
            }

            // Como el original: 1 - (z - (suelo + 100)) · 0.05 solo por encima, con dos llamadas a CalculateGroundHeight
            float heightMult = 1.f;
            if (TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP) + 100.f < ped->GetPosition().z) {
                heightMult = 1.f - (ped->GetPosition().z - (TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP) + 100.f)) * 0.05f;
                if (0.f > heightMult) {
                    heightMult = 0.f;
                }
            }
            const float mass = ped->m_fMass;
            ped->ApplyMoveForce(CVector{
                t[0] * MAGIC * heightMult * THRUST_FULL * mass * step,
                t[1] * MAGIC * heightMult * THRUST_FULL * mass * step,
                t[2] * MAGIC * heightMult * THRUST_FULL * mass * step
            });
        }

        if (m_ThrustStrafe != 0.f) { // 0x67F36F
            const CVector& right = ped->m_matrix->GetRight();
            const float mass = ped->m_fMass;
            ped->ApplyMoveForce(CVector{
                m_ThrustStrafe * right.x * MAGIC * THRUST_STRAFE * mass * step,
                m_ThrustStrafe * right.y * MAGIC * THRUST_STRAFE * mass * step,
                m_ThrustStrafe * right.z * MAGIC * THRUST_STRAFE * mass * step
            });
        }
    }

    JpSwingTailX87(this, ped, &prevPedVel, step);

    if (!m_bIsFinished) {
        ped->GetAE().TurnOnJetPack();
        ped->GetAE().UpdateJetPack((float)m_ThrustFwd, m_ThrustAngle);
    }
}

// 0x67B5C0
bool CTaskSimpleJetPack::ProcessAnims(CPed* ped) {
    if (CStreaming::IsModelLoaded(MODEL_JETPACK)) {
        if (!m_JetPackClump) {
            const auto mi = CModelInfo::GetModelInfo(MODEL_JETPACK);
            m_JetPackClump = reinterpret_cast<RpClump*>(mi->CreateInstance());
            mi->AddRef(); // Como el original: sin comprobar el clump creado
        }
    } else {
        CStreaming::RequestModel(MODEL_JETPACK, STREAMING_KEEP_IN_MEMORY);
    }

    if (!m_JetPackClump) {
        return false;
    }

    if (ped->bIsStanding) {
        if (ped->IsPlayer()) {
            ped->AsPlayer()->SetRealMoveAnim();
        }
    } else {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 4.f);
        ped->SetMoveState(PEDMOVE_STILL);
    }

    return true;
}

// 0x67E7B0
void CTaskSimpleJetPack::ProcessControlInput(CPlayerPed* player) {
    const auto pad = player->GetPadFromPlayer();
    // Como el original: GetWeaponSkill() sin tipo (0x5E6580) del arma activa
    const auto weaponType = player->m_aWeapons.data()[(int8)player->m_nActiveWeaponSlot].m_Type;
    const auto wi = CWeaponInfo::GetWeaponInfo(weaponType, player->GetWeaponSkill());

    if (wi->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT && wi->flags.bAimWithArm) { // 0x67E7E5
        if (const auto task = player->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)) {
            if (task->GetTaskType() == TASK_SIMPLE_PLAYER_ON_FOOT) {
                static_cast<CTaskSimplePlayerOnFoot*>(task)->ProcessPlayerWeapon(player);
            }
        }
    }

    // Como el original: estado de movimiento < PEDMOVE_WALK con signo
    if (player->bIsStanding && (int32)player->m_nMoveState < (int32)PEDMOVE_WALK && pad->GetExitVehicle()) { // 0x67E829
        DropJetPack(player);
        return;
    }

    const auto step = CTimer::GetTimeStep();
    const auto walkUpDown    = (float)pad->GetPedWalkUpDown();
    const auto walkLeftRight = (float)pad->GetPedWalkLeftRight();
    const auto padMoveMag    = std::sqrt(walkUpDown * walkUpDown + walkLeftRight * walkLeftRight) * kJpInv60;
    float      walkUpDownScaled = 0.f;

    const auto ClampStrafeUp = [this](float s, float limit) { m_ThrustStrafe = limit < s ? limit : s; };
    const auto ClampStrafeDown = [this](float s, float limit) { m_ThrustStrafe = limit > s ? limit : s; };
    const auto DecayStrafe = [&, this] { // 0x67ED69
        if (m_ThrustStrafe > 0.f) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.05f, 0.f);
        } else if (m_ThrustStrafe < 0.f) {
            ClampStrafeUp(step * 0.05f + m_ThrustStrafe, 0.f);
        }
    };

    if (CCamera::m_bUseMouse3rdPerson && !player->bIsStanding) { // 0x67E8BA
        const float walkLeftRightScaled = walkLeftRight * 0.0078125f;
        walkUpDownScaled = walkUpDown * 0.0078125f;
        // Como el original: fpatan del frente de la cámara; mirando a otro lado se invierte el vector (no se resta pi)
        const CVector front = TheCamera.GetActiveCam().m_vecFront;
        player->m_fAimingRotation = TheCamera.GetLookDirection() == LOOKING_DIRECTION_FORWARD
            ? JpAtan2X87(-front.x, front.y)
            : JpAtan2X87(front.x, -front.y);
        m_ThrustAngle = JpLerpThrustAngleX87(walkUpDownScaled, m_ThrustAngle, step);

        if (walkLeftRightScaled < 0.f) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.1f, -1.f);
        } else if (walkLeftRightScaled > 0.f) {
            ClampStrafeUp(step * 0.1f + m_ThrustStrafe, 1.f);
        } else if (m_ThrustStrafe > 0.f) {
            const float s = m_ThrustStrafe - step * 0.05f;
            m_ThrustStrafe = 0.f > s ? 0.f : s; // 0x420800
        } else if (m_ThrustStrafe < 0.f) {
            const float s = step * 0.05f + m_ThrustStrafe;
            m_ThrustStrafe = 0.f < s ? 0.f : s; // 0x404330
        }
    } else if (!player->m_pTargetedObject && !player->bIsStanding && !pad->GetTarget()) { // 0x67EA51
        const float walkLeftRightScaled = walkLeftRight * 0.0078125f;
        walkUpDownScaled = walkUpDown * 0.0078125f;
        if (walkLeftRightScaled != 0.f) { // 0x67EAA1
            // Como el original: ((lr / 128) · paso) · -0.05 + rumbo, con un solo ajuste de +-2pi
            const float a = walkLeftRightScaled * step * JETPACK_TURN_RATE + player->m_fAimingRotation;
            if (a > kJpPi) {
                player->m_fAimingRotation = a - kJpTwoPi;
            } else if (a < kJpMinusPi) {
                player->m_fAimingRotation = a + kJpTwoPi;
            } else {
                player->m_fAimingRotation = a;
            }
        }
        m_ThrustAngle = JpLerpThrustAngleX87(walkUpDownScaled, m_ThrustAngle, step); // Como el original: siempre en esta rama
    } else { // 0x67EB22
        if (padMoveMag > 0.f) {
            // Como el original: ángulo entre (0, 0) y (-lr, ud) menos la orientación de la cámara (0xB6F178), limitado
            const float angle = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkLeftRight, walkUpDown) - StaticRef<float>(0xB6F178);
            player->m_fAimingRotation = CGeneral::LimitRadianAngle(angle);
            m_ThrustAngle = THRUST_MAX_ANGLE * padMoveMag * -0.3515625f;
        } else {
            player->GetPlayerData()->m_fMoveBlendRatio = 0.f;
        }
    }

    if (player->m_pTargetedObject && !player->bIsStanding) {
        // Como el original: límite |dz| · 0.2 + 3, divisor max(dist, 0.1) por recíproco, (1 - dist / límite) · 3, masa y paso
        const CVector& targetPos = player->m_pTargetedObject->GetPosition();
        const CVector& playerPos = player->GetPosition();
        const float dx = playerPos.x - targetPos.x;
        const float dy = playerPos.y - targetPos.y;
        const float dz = playerPos.z - targetPos.z;
        const float limit = std::abs(dz) * 0.2f + 3.f;
        const float dist2D = std::sqrt(dx * dx + dy * dy);
        if (dist2D < limit) {
            const float inv = 1.f / (0.1f > dist2D ? 0.1f : dist2D);
            const float k = (1.f - dist2D / limit) * 3.f;
            const float x = dx * inv, y = dy * inv, z = inv * 0.f;
            const float mass = player->m_fMass;
            player->ApplyMoveForce(CVector{
                x * k * MAGIC * THRUST_FULL * mass * step,
                y * k * MAGIC * THRUST_FULL * mass * step,
                z * k * MAGIC * THRUST_FULL * mass * step
            });
        }
    }

    if (CCamera::m_bUseMouse3rdPerson && !player->bIsStanding) { // 0x67ED2D
        m_ThrustStop = false;
        if (pad->GetBrake()) {
            m_ThrustFwd = -1;
        } else {
            m_ThrustFwd = (char)((pad->GetAccelerate() || walkUpDownScaled != 0.f) ? 1 : 0);
        }
        DecayStrafe();
    } else if (pad->GetLookBehindForCar()) { // 0x67EDA4
        m_ThrustStop   = true;
        m_ThrustFwd    = 0;
        m_ThrustStrafe = 0.f;
    } else {
        m_ThrustStop = false;
        // Como el original: sin ratón solo acelera con el botón (no con el stick)
        m_ThrustFwd  = (char)(pad->GetBrake() ? -1 : (pad->GetAccelerate() ? 1 : 0));
        if (pad->GetLookLeft() && !pad->GetTarget()) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.1f, -1.f);
        } else if (pad->GetLookRight() && !pad->GetTarget()) {
            ClampStrafeUp(step * 0.1f + m_ThrustStrafe, 1.f);
        } else {
            DecayStrafe();
        }
    }

    // 0x67EEA9
    auto& br = player->GetPlayerData()->m_fMoveBlendRatio;
    if (padMoveMag > 0.f) {
        const auto delta = step * 0.07f;
        br = padMoveMag - br > delta
            ? delta + br
            : padMoveMag;
    } else {
        br = 0.f;
    }
}

// 0x67B720
void CTaskSimpleJetPack::ApplyRollAndPitch(CPed* ped) {
    if (ped->bIsStanding || ped->bWasStanding) {
        return;
    }
    for (const auto nodeId : { PED_NODE_LEFT_LEG, PED_NODE_RIGHT_LEG }) { // Rotate legs according to current swing values
        const auto q = &ped->m_apBones[nodeId]->KeyFrame->q;
        RtQuatRotate(q, &CPedIK::ZaxisIK, m_LegSwingFwd * kJpRadToDeg, rwCOMBINEPOSTCONCAT); // Como el original: 180/pi del EXE
        RtQuatRotate(q, &CPedIK::YaxisIK, m_LegSwingSide * kJpRadToDeg, rwCOMBINEPOSTCONCAT);
    }
    ped->bUpdateMatricesRequired = true;
}

// 0x67B660
void CTaskSimpleJetPack::DropJetPack(CPed* ped) {
    if (!m_bIsFinished) {
        CVector pickupPos;
        CPickups::CreatePickupCoorsCloseToCoors(ped->GetPosition(), pickupPos);
        CPickups::GenerateNewOne(pickupPos, MODEL_JETPACK, PICKUP_ONCE_TIMEOUT_SLOW, 0, 0, true, nullptr);
    }
    MakeAbortable(ped);
    m_ThrustStop   = 0;
    m_ThrustFwd    = 0;
    m_ThrustStrafe = 0.f;
}

// 0x67B7F0
void CTaskSimpleJetPack::DoJetPackEffect(CPed* ped) {
    // Como el original: primero se crean los dos efectos (el tiempo clave vuelve a 0 al crear cada uno),
    // después se colocan y al final se acerca el tiempo clave a su objetivo de 0.1 en 0.1 sin pasarse.
    if (const auto clump = ped->GetRpClump()) {
        if (const auto mat = RwFrameGetMatrix(RpClumpGetFrame(clump))) {
            for (auto& fx : m_ThrusterFX) {
                if (fx) {
                    continue;
                }
                const CVector zero{ 0.f, 0.f, 0.f };
                fx = g_fxMan.CreateFxSystem("jetpack", zero, mat, false);
                if (fx) {
                    fx->Play();
                    fx->SetLocalParticles(true);
                    fx->CopyParentMatrix();
                    m_FxKeyTime = 0.f;
                }
            }
        }
    }
    const char* const jbFrameNames[2]{ "jetball1", "jetball2" };
    for (size_t i = 0; i < 2; ++i) {
        if (const auto fx = m_ThrusterFX[i]) {
            fx->SetMatrix(RwFrameGetLTM(CClumpModelInfo::GetFrameFromName(m_JetPackClump, jbFrameNames[i])));
            fx->SetOffsetPos(JETBALL_OFFSET);
        }
    }

    // When pressing the brake button (space) you're going down faster.
    // But the flame is blue, even though the audio for the blue flame isn't playing.
    // So, going by the logic "blue == thrust is generated", "orange == no thrust generated"
    // I conclude it this is a bug.
    const bool  thrusting = (notsa::IsFixBugs() ? m_ThrustFwd == 1 : m_ThrustFwd != 0) || m_ThrustStrafe != 0.f;
    const float target    = thrusting ? 1.f : 0.f;
    if (target > m_FxKeyTime) {
        m_FxKeyTime += 0.1f;
        if (m_FxKeyTime > 1.f) {
            m_FxKeyTime = 1.f;
        }
    } else if (target < m_FxKeyTime) {
        m_FxKeyTime -= 0.1f;
        if (m_FxKeyTime < 0.f) {
            m_FxKeyTime = 0.f;
        }
    }
    for (const auto fx : m_ThrusterFX) {
        if (fx) {
            fx->SetConstTime(true, m_FxKeyTime);
        }
    }
}

// 0x67BA10
void CTaskSimpleJetPack::StopJetPackEffect() {
    for (auto& fx : m_ThrusterFX) {
        if (fx) {
            fx->Kill();
            fx = nullptr;
        }
    }
}
