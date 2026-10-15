/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CQuadBike
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "QuadBike.h"
#include "VehicleRecording.h"
#include "ControllerConfigManager.h"
#include "X87Math.h"

auto& bDoQuadDamping = StaticRef<bool>(0x8D3450); // true
auto& QUAD_HBSTEER_ANIM_MULT = StaticRef<float>(0x8D3454); // -0.4f
auto& vecQuadResistance = StaticRef<CVector>(0x8D3458); // { 0.995f, 0.995f, 1.0f }

namespace {
// Como el original: Multiply3x3 (0x59C790), matriz por vector, con el orden de sumas del EXE
CVector QuadMultiply3x3_MV(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: Multiply3x3 (0x59C810), vector por matriz (no es la inversa), con el orden de sumas del EXE
CVector QuadMultiply3x3_VM(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}

// Como el original: plantillas Max (0x420800) y Min (0x404330) del EXE; con NaN devuelven el segundo
float QuadMax(float a, float b) { return a > b ? a : b; }
float QuadMin(float a, float b) { return a < b ? a : b; }
} // namespace

void CQuadBike::InjectHooks() {
    RH_ScopedVirtualClass(CQuadBike, 0x871ae8, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6CE370);
    RH_ScopedVMTInstall(Fix, 0x6CE2B0);
    RH_ScopedVMTInstall(GetRideAnimData, 0x6CDC90);
    RH_ScopedVMTInstall(PreRender, 0x6CEAD0);
    RH_ScopedVMTInstall(ProcessAI, 0x6CE460);
    RH_ScopedVMTInstall(ProcessControl, 0x6CDCC0);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6CE020);
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6CE280);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6CE270);
    RH_ScopedVMTInstall(ResetSuspension, 0x6CDCB0);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6CE340);
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6CDCA0);
}

// 0x6CE370
CQuadBike::CQuadBike(int32 modelIndex, eVehicleCreatedBy createdBy) :
    CAutomobile(modelIndex, createdBy, false)
{
    // Como el original (0x6CE3B9): el modelo sale del indice recibido (no de m_nModelIndex) y el manejo se indexa con el byte bajo de m_nHandlingId
    auto* const mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    m_pHandling = gHandlingDataMgr.GetBikeHandlingPointer(mi->m_nHandlingId);
    m_nVehicleSubType = VEHICLE_TYPE_QUAD;

    { // unused
        field_9A8[0] = 1.f;
        field_9A8[1] = 0.f;
        field_9A8[2] = 0.f;
        field_9A8[3] = 0.0f; // see SetupSuspensionLines
    }

    CQuadBike::SetupSuspensionLines();

    m_nQuadFlags &= ~1u; // useless
    m_fSteerAngle = 0.0f;
}

CQuadBike* CQuadBike::Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) {
    this->CQuadBike::CQuadBike(modelIndex, createdBy);
    return this;
}

// 0x6CE2B0
void CQuadBike::Fix() {
    // Como el original: primero se restablece todo el daño (0x6C20E0) y luego se quitan puertas 2..5 y ruedas a OK
    m_damageManager.ResetDamageStatus();
    for (auto i = 2; i < 6; i++) { // from DOOR_LEFT_FRONT to MAX_DOORS
        m_damageManager.SetDoorStatus(static_cast<eDoors>(i), eDoorStatus::DAMSTATE_NOTPRESENT);
    }
    vehicleFlags.bIsDamaged = false;
    RpClumpForAllAtomics(GetRpClump(), CVehicleModelInfo::HideAllComponentsAtomicCB, (void*)2); // TODO Use RpAtomicVisibility::VISIBILITY_DAM instead of `2` here
    for (auto i = 0; i < 4; i++) {
        m_damageManager.SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_OK);
    }
}

// 0x6CDC90
CRideAnimData* CQuadBike::GetRideAnimData() {
    return &m_sRideAnimData;
}

// 0x6CEAD0
void CQuadBike::PreRender() {
    CAutomobile::PreRender();

    auto mi = GetVehicleModelInfo();
    CVector wheelPos;
    mi->GetWheelPosn(CAR_WHEEL_REAR_LEFT, wheelPos, false);
    SetTransmissionRotation(
        m_aCarNodes[QUAD_REAR_AXLE],
        m_wheelPosition[CAR_WHEEL_REAR_LEFT],
        m_wheelPosition[CAR_WHEEL_REAR_RIGHT],
        wheelPos,
        false
    );

    CVector wheelFrontLeftPos;
    mi->GetWheelPosn(CAR_WHEEL_FRONT_LEFT, wheelFrontLeftPos, false);

    // Como el original: se adjunta la matriz RW del marco, se guarda su posición, se fija una rotación completa
    // (SetRotateY/SetRotateZ ponen la posición a cero) y se le suma la posición guardada
    CMatrix mat;

    if (m_aCarNodes[QUAD_SUSPENSION_LF]) {
        mat.Attach(RwFrameGetMatrix(m_aCarNodes[QUAD_SUSPENSION_LF]), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotateY(TrigAtan2Cpp(m_wheelPosition[CAR_WHEEL_FRONT_LEFT] - wheelFrontLeftPos.z, std::fabs(wheelFrontLeftPos.x)));
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    if (m_aCarNodes[QUAD_SUSPENSION_RF]) {
        mat.Attach(RwFrameGetMatrix(m_aCarNodes[QUAD_SUSPENSION_RF]), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotateY(-TrigAtan2Cpp(m_wheelPosition[eCarWheel::CAR_WHEEL_FRONT_RIGHT] - wheelFrontLeftPos.z, std::fabs(wheelFrontLeftPos.x)));
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    if (m_aCarNodes[QUAD_HANDLEBARS]) {
        mat.Attach(RwFrameGetMatrix(m_aCarNodes[QUAD_HANDLEBARS]), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotateZ(QUAD_HBSTEER_ANIM_MULT * m_sRideAnimData.AnimLeanLeft);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }
}

// 0x6CE460
bool CQuadBike::ProcessAI(uint32& extraHandlingFlags) {
    if (GetStatus() != STATUS_PLAYER) {
        return CAutomobile::ProcessAI(extraHandlingFlags);
    }

    GetColModel(); // Called, but not used?

    m_autoPilot.carCtrlFlags.bHonkAtCar = false;
    m_autoPilot.carCtrlFlags.bHonkAtPed = false;
    const auto recID = m_autoPilot.m_vehicleRecordingId;
    if (recID >= 0 && !CVehicleRecording::bUseCarAI.data()[recID]) {
        return false;
    }
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    if (!m_pDriver || !m_pDriver->IsPlayer()) {
        return false;
    }

    PruneReferences();
    // Como el original: primero las tres tareas (0x329, 0x337, 0x339) y despues el estado "arrestado"
    if (m_pDriver->GetTaskManager().HasAnyOf<TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, TASK_COMPLEX_CAR_QUICK_BE_DRAGGED_OUT>()
        || m_pDriver->m_nPedState == PEDSTATE_ARRESTED
    ) {
        vehicleFlags.bIsHandbrakeOn = true;
        m_BrakePedal = 1.0f;
        m_GasPedal = 0.0f;
        return false;
    }

    // Como el original: se vuelve a comprobar al conductor jugador antes de leer sus controles
    CPad* pad = nullptr;
    if (m_pDriver && m_pDriver->IsPlayer()) {
        ProcessControlInputs((uint8)m_pDriver->m_nPedType);
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    const float timeStep = CTimer::GetTimeStep();
    float fTurnForcePerTimeStep = 0.0f;
    const float fLeanDirection = DotProduct(m_vecTurnSpeed, m_matrix->GetRight());

    // Como el original: Magnitude se llama dos veces cuando no supera 0.1 (Min de 0x404330 con el segundo operando)
    const auto LimitedSpeed = [this]() -> float {
        const float mag = m_vecMoveSpeed.Magnitude();
        return !(mag > 0.1f) ? m_vecMoveSpeed.Magnitude() : 0.1f;
    };

    if (m_sRideAnimData.LeanFwd < 0.0f && fLeanDirection < m_pHandling->m_fLeanBakCOM) {
        // Lean back
        if (m_nNumContactWheels == 0) {
            fTurnForcePerTimeStep = m_fTurnMass * m_sRideAnimData.LeanFwd * 0.0015f;
        } else if (m_BrakePedal == 0.0f && !vehicleFlags.bIsHandbrakeOn) {
            fTurnForcePerTimeStep = m_pHandling->m_fLeanBakForce * m_fTurnMass * m_sRideAnimData.LeanFwd * LimitedSpeed();
        }
        const float fTurnForce = -(timeStep * fTurnForcePerTimeStep);
        ApplyTurnForce(m_matrix->GetUp() * fTurnForce, m_vecCentreOfMass + m_matrix->GetForward());
    } else if (m_sRideAnimData.LeanFwd > 0.0f && fLeanDirection > -m_pHandling->m_fLeanFwdCOM) {
        // Lean forward
        if (m_nNumContactWheels == 0) {
            fTurnForcePerTimeStep = m_fTurnMass * m_sRideAnimData.LeanFwd * 0.0015f;
        } else if (m_BrakePedal > 0.0f) {
            fTurnForcePerTimeStep = m_pHandling->m_fLeanFwdForce * m_fTurnMass * m_sRideAnimData.LeanFwd * LimitedSpeed();
        }
        const float fTurnForce = -(timeStep * fTurnForcePerTimeStep);
        ApplyTurnForce(m_matrix->GetUp() * fTurnForce, m_vecCentreOfMass + m_matrix->GetForward());
    }

    // Como el original: solo con las cuatro ruedas en el aire (compresion exactamente 1.0) y con mando
    if (m_fWheelsSuspensionCompression[0] == 1.0f && m_fWheelsSuspensionCompression[1] == 1.0f
        && m_fWheelsSuspensionCompression[2] == 1.0f && m_fWheelsSuspensionCompression[3] == 1.0f
        && pad
    ) {
        float steeringLeftRightProgress = float(int32(pad->GetSteeringLeftRight())) * 0.0078125f;
        if (CCamera::m_bUseMouse3rdPerson && std::fabs(steeringLeftRightProgress) < 0.05f) {
            steeringLeftRightProgress = QuadMin(1.5f, QuadMax(-1.5f, CPad::NewMouseControllerState.m_AmountMoved.x * 0.01f));
        }
        if (vehicleFlags.bIsHandbrakeOn) {
            const float fTurnSpeed_Dot_MatUp = DotProduct(m_vecTurnSpeed, m_matrix->GetUp());
            if ((fTurnSpeed_Dot_MatUp < 0.03f && steeringLeftRightProgress < 0.0f)
                || (fTurnSpeed_Dot_MatUp > -0.03f && steeringLeftRightProgress > 0.0f)
            ) {
                const float fTurnForce = timeStep * m_fTurnMass * 0.0015f * steeringLeftRightProgress;
                ApplyTurnForce(m_matrix->GetRight() * fTurnForce, m_vecCentreOfMass + m_matrix->GetForward());
            }
        } else if (!pad->GetAccelerate()) {
            const float fTurnSpeed_Dot_MatFwd = DotProduct(m_vecTurnSpeed, m_matrix->GetForward());
            if ((fTurnSpeed_Dot_MatFwd < 0.03f && steeringLeftRightProgress < 0.0f)
                || (fTurnSpeed_Dot_MatFwd > -0.03f && steeringLeftRightProgress > 0.0f)
            ) {
                const float fTurnForce = timeStep * m_fTurnMass * 0.0015f * steeringLeftRightProgress;
                ApplyTurnForce(m_matrix->GetRight() * fTurnForce, m_vecCentreOfMass + m_matrix->GetUp());
            }
        }
    }

    // Como el original: pow del EXE (0x822130) a precision completa
    m_sRideAnimData.LeanAngle = QuadBikeLeanAngleCpp(
        m_pHandling->m_fDesLean,
        timeStep,
        m_sRideAnimData.LeanAngle,
        m_pHandling->m_fFullAnimLean,
        m_fSteerAngle,
        m_pHandlingData->m_fSteeringLock
    );

    DoDriveByShootings();

    if (CCheat::IsActive(CHEAT_CARS_ON_WATER)) {
        DoHoverSuspensionRatios(); // Como el original: 0x6CEA9A llama al slot 69 (0x6A45C0), no a CEntity::Remove
    }
    return false;
}

// 0x6CDCC0
void CQuadBike::ProcessControl() {
    if (GetStatus() == STATUS_PLAYER && bDoQuadDamping) {
        const CVector resistance = vecQuadResistance;
        const float   timeStep   = CTimer::GetTimeStep();
        // Como el original: Multiply3x3 vector * matriz (0x59C810) de la velocidad de giro
        const CVector velocity = QuadMultiply3x3_VM(m_vecTurnSpeed, *m_matrix);
        float resistanceX;
        float speedResistanceX;
        const auto& compression = m_fWheelsSuspensionCompression;
        if (compression[CAR_WHEEL_FRONT_LEFT] == 1.0f && compression[CAR_WHEEL_FRONT_RIGHT] == 1.0f) {
            speedResistanceX = 0.995f;
            if ((compression[CAR_WHEEL_REAR_LEFT] < 1.0f || compression[CAR_WHEEL_REAR_RIGHT] < 1.0f) && m_matrix->GetForward().z > 0.0f) {
                const float v = std::fabs(m_pHandling->m_fWheelieAng - m_matrix->GetForward().z) * 0.25f;
                resistanceX = resistance.x - (0.07f < v ? 0.07f : v);
            } else {
                resistanceX = 0.995f;
            }
        } else if (m_WheelCounts[CAR_WHEEL_REAR_LEFT] == 1.0f && m_WheelCounts[CAR_WHEEL_REAR_RIGHT] == 1.0f) {
            speedResistanceX = 0.995f;
            if (m_matrix->GetForward().z < 0.0f) {
                const float v = std::fabs(m_pHandling->m_fStoppieAng - m_matrix->GetForward().z) * 0.3f;
                resistanceX = (0.9f + (0.1f < v ? 0.1f : v)) * resistance.x;
            } else {
                resistanceX = resistance.x;
            }
        } else {
            speedResistanceX = 0.5f;
            resistanceX = resistance.x;
        }
        resistanceX /= velocity.x * velocity.x * speedResistanceX + 1.0f;
        const float resistanceY = resistance.y / (velocity.y * velocity.y * 1.0f + 1.0f);
        const float deltaX = QuadBikeDampDeltaStoredCpp(resistanceX, timeStep, velocity.x);
        const float deltaY = QuadBikeDampDeltaFullCpp(resistanceY, timeStep, velocity.y);

        // Como el original: fuerza = (-up * delta) * masa de giro, con cada producto redondeado a float
        const float turnMass = m_fTurnMass;
        const CMatrix& mat   = *m_matrix;
        {
            const float fx = -mat.GetUp().x * deltaY;
            const float fy = -mat.GetUp().y * deltaY;
            const float fz = -mat.GetUp().z * deltaY;
            const CVector point = QuadMultiply3x3_MV(mat, m_vecCentreOfMass);
            ApplyTurnForce(
                CVector{ fx * turnMass, fy * turnMass, fz * turnMass },
                CVector{ point.x + mat.GetRight().x, point.y + mat.GetRight().y, point.z + mat.GetRight().z }
            );
        }
        {
            const float fx = deltaX * mat.GetUp().x;
            const float fy = deltaX * mat.GetUp().y;
            const float fz = deltaX * mat.GetUp().z;
            const CVector point = QuadMultiply3x3_MV(*m_matrix, m_vecCentreOfMass);
            ApplyTurnForce(
                CVector{ fx * turnMass, fy * turnMass, fz * turnMass },
                CVector{ mat.GetForward().x + point.x, mat.GetForward().y + point.y, mat.GetForward().z + point.z }
            );
        }
    }
    CAutomobile::ProcessControl();
}

// 0x6CE020
void CQuadBike::ProcessControlInputs(uint8 playerNum) {
    CAutomobile::ProcessControlInputs(playerNum);

    // Como el original: la inclinacion hacia delante se acerca a la entrada (-arriba/abajo / 128) con paso 0.2 (no / 5)
    const auto KeyboardLean = [&] {
        const float target = float(-int32(CPad::GetPad(playerNum)->GetSteeringUpDown())) * 0.0078125f;
        m_sRideAnimData.LeanFwd = (target - m_sRideAnimData.LeanFwd) * CTimer::GetTimeStep() * 0.2f + m_sRideAnimData.LeanFwd;
    };

    if (!CCamera::m_bUseMouse3rdPerson || !m_bEnableMouseSteering) {
        KeyboardLean();
    } else {
        bool useMouse = true;
        if (CPad::NewMouseControllerState.m_AmountMoved.x == 0.0f && CPad::NewMouseControllerState.m_AmountMoved.y == 0.0f) {
            const bool controlsKept = std::fabs(m_fRawSteerAngle) > 0.0f
                && m_nLastControlInput == eControllerType::MOUSE
                && CPad::GetPad(playerNum)->GetSteeringLeftRight() == 0;
            if (!controlsKept || CPad::GetPad(playerNum)->GetSteeringUpDown() != 0) {
                useMouse = false;
                if (CPad::GetPad(playerNum)->GetSteeringUpDown() != 0 || m_nLastControlInput != eControllerType::MOUSE) {
                    m_nLastControlInput = eControllerType::KEYBOARD;
                    KeyboardLean();
                }
            }
        }
        if (useMouse) {
            m_nLastControlInput = eControllerType::MOUSE;
            if (CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook == 0) {
                m_sRideAnimData.LeanFwd = CPad::NewMouseControllerState.m_AmountMoved.y * -0.0035f + m_sRideAnimData.LeanFwd;
            }
            if (std::fabs(m_sRideAnimData.LeanFwd) < 0.35f || CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook != 0) {
                PowScaleFloatCpp(&m_sRideAnimData.LeanFwd, 0.98f, CTimer::GetTimeStep());
            }
        }
    }

    // Como el original: Min/Max inline, con NaN queda -1.0
    {
        float& lean = m_sRideAnimData.LeanFwd;
        if (lean > -1.0f) {
            if (!(lean < 1.0f)) {
                lean = 1.0f;
            }
        } else {
            lean = -1.0f;
        }
    }
    if (CPad::GetPad(playerNum)->DisablePlayerControls) {
        m_sRideAnimData.LeanFwd = 0.0f;
    }
}

// 0x6CE280
void CQuadBike::ProcessDrivingAnims(CPed* driver, bool blend) {
    if (!m_bOffscreen) { // see CBike::ProcessDrivingAnims
        CBike::ProcessRiderAnims(driver, this, &m_sRideAnimData, m_pHandling, 0);
    }
}

// 0x6CE270
void CQuadBike::ProcessSuspension() {
    CAutomobile::ProcessSuspension();
}

// 0x6CDCB0
void CQuadBike::ResetSuspension() {
    CAutomobile::ResetSuspension();
}

// 0x6CE340
void CQuadBike::SetupDamageAfterLoad() {
    if (m_aCarNodes[QUAD_BODY_FRONT]) {
        CAutomobile::SetBumperDamage(FRONT_BUMPER, false);
    }
    if (m_aCarNodes[QUAD_BODY_REAR]) {
        CAutomobile::SetDoorDamage(DOOR_BONNET, false);
    }
}

// 0x6CDCA0
void CQuadBike::SetupSuspensionLines() {
    field_9A8[0] = 1.0f;
    CAutomobile::SetupSuspensionLines();
}
