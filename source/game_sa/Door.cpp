/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDoor
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
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

#include "Door.h"

float DOOR_SPEED_MIN_LIMIT = 0.001f; // 0x872304
float DOOR_SPEED_MIN_LIMIT_TRAIN = 0.002f; // 0x872308
float DOOR_DAMP_RATE = 0.97f; // 0x87230C
float DOOR_DAMP_RATE_BOAT = 0.98f; // 0x872310
float DOOR_DAMP_RATE_FIRETRUCK = 0.92f; // 0x872314
float DOOR_DAMP_RATE_BONNET = 0.97f; // 0x872318

float DOOR_DAMP_RATE_CHASSIS = 0.98f; // 0x87231C
float DOOR_DAMP_RATE_CHASSIS_CUTSCENE = 0.95f; // 0x872320
float DOOR_SPRING_RATE_CHASSIS = 0.06f; // 0x872324
float DOOR_APPLY_RATE_CHASSIS = 0.025f; // 0x872328
float DOOR_APPLY_RATE_CHASSIS_CUTSCENE = 0.0f; // 0x87232C
float DOOR_SPEED_MAX_CAPPED_CHASSIS = 0.02f; // 0x872330
float DOOR_SPEED_MAX_CAPPED_FIRETRUCK = 0.05f; // 0x872334
float DOOR_SPRING_RATE_FIRETRUCK = 0.01f; //0x872338
float DOOR_SLAM_SHUT_SPEED = 0.1f; // 0x87233C

void CDoor::InjectHooks() {
    RH_ScopedClass(CDoor);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(Process, 0x6F4040);
    RH_ScopedGlobalInstall(ProcessImpact, 0x6F4540);
    RH_ScopedGlobalInstall(Open, 0x6F4790);
    RH_ScopedGlobalInstall(GetAngleOpenRatio, 0x6F47E0);
    RH_ScopedGlobalInstall(IsClosed, 0x6F4800);
    RH_ScopedGlobalInstall(IsFullyOpen, 0x6F4820);
}

namespace {
// Como el original: fsin/fcos del x87 a precisión interna completa, redondeados a float al guardarlos
float DoorSinX87(float a) {
    float r;
    __asm {
        fld a
        fsin
        fstp r
    }
    return r;
}

float DoorCosX87(float a) {
    float r;
    __asm {
        fld a
        fcos
        fstp r
    }
    return r;
}

// Como el original: Multiply3x3 (0x59C810), vector por matriz, con el orden de sumas del EXE
CVector DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}

// Como el original: angVel = pow(rate, timestep) * angVel con el _CIpow del EXE (0x822130)
void DoorMultiplyByPow(float& angVel, float rate, float step) {
    float* const p = &angVel;
    __asm {
        fld rate
        fld step
        mov eax, 0x822130
        call eax
        mov edx, p
        fmul dword ptr [edx]
        fstp dword ptr [edx]
    }
}
} // namespace

// 0x6F4040
bool CDoor::Process(CVehicle* vehicle, Const CVector& oldMoveSpeed, Const CVector& oldTurnSpeed, Const CVector& offset) {
    CVector velocity = vehicle->GetSpeed(offset);
    CVector oldVelocity = oldTurnSpeed.Cross(offset) + oldMoveSpeed;

    CVector acceleration = velocity - oldVelocity;

    acceleration.z += CTimer::GetTimeStep() * ((m_dirn & DOOR_EXTRA_LOW_GRAVITY) ? std::bit_cast<float>(0x3AD1B718u) : 0.008f); // TODO: Gravity. Como el original: 0x87149C es el float siguiente a 0.0016

    CMatrix mat(*vehicle->m_matrix); // Como el original: copia la matriz sin reservarla
    acceleration = DoorMultiply3x3_VM(acceleration, mat);

    CVector vecDirn{};
    switch (m_dirn & DOOR_EXTRA_DIRN_MASK) {
    case DOOR_AXIS_X:     vecDirn.x = 1.0f; break;
    case DOOR_AXIS_Y:     vecDirn.y = 1.0f; break;
    case DOOR_AXIS_Z:     vecDirn.z = 1.0f; break;
    case DOOR_AXIS_NEG_X: vecDirn.x = -1.0f; break;
    case DOOR_AXIS_NEG_Y: vecDirn.y = -1.0f; break;
    case DOOR_AXIS_NEG_Z: vecDirn.z = -1.0f; break;
    default:              break; // Como el original: el vector queda a cero
    }

    // 0x6F41B2
    const float sin = DoorSinX87(m_angle);
    const float cos = DoorCosX87(m_angle);

    // Como el original: solo se rota en los ejes X y Z; con cualquier otro eje se usa el seno
    float ang;
    if (m_axis == DOOR_AXIS_X || m_axis == DOOR_AXIS_Z) {
        const auto a = (m_axis + 1) % 3;
        const auto b = (m_axis + 2) % 3;

        const float newA = cos * vecDirn[a] - sin * vecDirn[b];
        const float newB = cos * vecDirn[b] + sin * vecDirn[a];
        vecDirn[a] = newA;
        vecDirn[b] = newB;

        ang = acceleration.Cross(vecDirn)[m_axis];
    } else {
        ang = sin;
    }

    // 0x6F42A8
    // Angular velocity update
    if (m_dirn & DOOR_EXTRA_CHASSIS) {
        // Cutscene or airborne: use reduced apply rate
        const float rate = (TheCamera.m_bWideScreenOn || (vehicle->IsAutomobile() && !vehicle->AsAutomobile()->m_nNumContactWheels))
            ? DOOR_APPLY_RATE_CHASSIS_CUTSCENE
            : DOOR_APPLY_RATE_CHASSIS;
        m_angVel += rate * ang;
    } else if (std::abs(ang) > DOOR_SPEED_MIN_LIMIT || std::abs(m_angVel) > DOOR_SPEED_MIN_LIMIT) {
        m_angVel += ang;
    }

    // 0x6F42E8
    // Attenuation (damping + spring)
    if (m_dirn & DOOR_EXTRA_CHASSIS) {
        const float rate = (TheCamera.m_bWideScreenOn || (vehicle->IsAutomobile() && !vehicle->AsAutomobile()->m_nNumContactWheels))
            ? DOOR_DAMP_RATE_CHASSIS_CUTSCENE
            : DOOR_DAMP_RATE_CHASSIS;
        DoorMultiplyByPow(m_angVel, rate, CTimer::GetTimeStep());
        m_angVel = m_angVel - CTimer::GetTimeStep() * m_angle * DOOR_SPRING_RATE_CHASSIS;
    } else if (vehicle->IsBoat()) {
        m_angVel *= DOOR_DAMP_RATE_BOAT;
    } else if (vehicle->GetModelIndex() == MODEL_FIRELA) {
        m_angVel = DOOR_DAMP_RATE_FIRETRUCK * m_angVel
            - CTimer::GetTimeStep() * m_angle * DOOR_SPRING_RATE_FIRETRUCK;
    } else if (m_dirn & DOOR_EXTRA_LOW_GRAVITY) {
        m_angVel *= DOOR_DAMP_RATE_BONNET;
    } else {
        m_angVel *= DOOR_DAMP_RATE;
    }

    // Speed limit
    float capSpeed = (m_dirn & DOOR_EXTRA_CHASSIS) ? DOOR_SPEED_MAX_CAPPED_CHASSIS
        : (m_dirn & DOOR_EXTRA_FIRETRUCK)          ? DOOR_SPEED_MAX_CAPPED_FIRETRUCK
                                                   : CDoor::DOOR_SPEED_MAX_CAPPED;

    m_angVel = std::clamp(m_angVel, -capSpeed, capSpeed);

    // Integration of an angle
    m_angle += m_angVel;

    if (!(m_dirn & DOOR_EXTRA_FIXEDSTATE)) {
        m_doorState = DOOR_NOTHING;
    }

    // Checking corner angles
    const bool posOpenRotation = !(m_openAngle < m_closedAngle); // Como el original: NaN cuenta como apertura positiva
    const bool hitOpen = posOpenRotation ? (m_angle > m_openAngle) : (m_angle < m_openAngle);
    const bool hitClosed = posOpenRotation ? (m_angle < m_closedAngle) : (m_angle > m_closedAngle);

    if (hitOpen) {
        // 0x6F446E
        m_angVel *= -0.8f; // TODO:: define?
        m_angle = m_openAngle;
        if (!(m_dirn & DOOR_EXTRA_FIXEDSTATE)) {
            m_doorState = DOOR_HIT_MAX_END;
        }
    } else if (hitClosed) {
        // 0x6F4492
        m_angle = m_closedAngle;
        if ((m_dirn & DOOR_EXTRA_BASED) && !(m_dirn & DOOR_EXTRA_FIXEDSTATE)
            && std::abs(m_angVel) > DOOR_SLAM_SHUT_SPEED) {
            m_angVel = 0.0f;
            m_doorState = DOOR_SLAM_SHUT;
            return true;
        }
        m_angVel *= (m_dirn & DOOR_EXTRA_LOW_GRAVITY) ? -0.9f : -0.4f; // TODO:: define?
        if (!(m_dirn & DOOR_EXTRA_FIXEDSTATE)) {
            m_doorState = DOOR_HIT_MIN_END;
        }
    }

    return false;
}

// 0x6F4540
bool CDoor::ProcessImpact(CVehicle* vehicle, Const CVector& oldMoveSpeed, Const CVector& oldTurnSpeed, Const CVector& offset) {
    static const float POP_OPEN_DOOR_ACCEL_LIMIT = 0.1f; // 0x8D3968

    CVector velocity = vehicle->GetSpeed(offset);
    CVector oldVelocity = oldTurnSpeed.Cross(offset) + oldMoveSpeed;
    CVector acceleration = velocity - oldVelocity;

    CMatrix mat(*vehicle->m_matrix); // Como el original: copia la matriz sin reservarla

    CVector direction;
    switch (m_dirn & DOOR_EXTRA_DIRN_MASK) {
    case DOOR_AXIS_X:     direction = vehicle->GetRight(); break;
    case DOOR_AXIS_Y:     direction = vehicle->GetForward(); break;
    case DOOR_AXIS_Z:     direction = vehicle->GetUp(); break;
    case DOOR_AXIS_NEG_X: direction = -vehicle->GetRight(); break;
    case DOOR_AXIS_NEG_Y: direction = -vehicle->GetForward(); break;
    case DOOR_AXIS_NEG_Z: direction = -vehicle->GetUp(); break;
    default:              NOTSA_UNREACHABLE(); break;
    }

    // Angular velocity along the desired axis
    // Como el original: solo los ejes X y Z usan el producto vectorial; con cualquier otro eje se compara
    // la componente Y de la velocidad anterior (el hueco de pila que el EXE reutiliza)
    float ang = oldVelocity.y;
    if (m_axis == DOOR_AXIS_X || m_axis == DOOR_AXIS_Z) {
        ang = acceleration.Cross(direction)[m_axis];
    }

    const float limit = CGeneral::GetRandomNumberInRange(0.75f, 1.5f) * POP_OPEN_DOOR_ACCEL_LIMIT;

    // Is there enough acceleration to open the door?
    const bool posOpenRotation = !(m_openAngle < m_closedAngle); // Como el original: NaN cuenta como apertura positiva
    if (posOpenRotation) {
        return ang > limit;
    } else {
        return ang < -limit;
    }
}

// 0x6F4790
void CDoor::Open(float angRatio) {
    m_prevAngle = m_angle;
    if (!(angRatio < 1.0f)) { // Como el original: NaN abre del todo
        m_angle = m_openAngle;
        if (!(m_dirn & DOOR_EXTRA_FIXEDSTATE)) {
            m_doorState = DOOR_HIT_MAX_END;
        }
    } else {
        m_angle = angRatio * m_openAngle;
        if (m_angle == 0.0f) {
            m_angVel = 0.0f;
        }
    }
}

// 0x6F47E0
float CDoor::GetAngleOpenRatio() const {
    if (m_openAngle != 0.0f) {
        return m_angle / m_openAngle;
    }

    return 0.0f;
}

// 0x6F4800
bool CDoor::IsClosed() const {
    return m_closedAngle == m_angle;
}

// 0x6F4820
bool CDoor::IsFullyOpen() const {
    return !(std::fabsf(GetAngleWhenOpen()) - 0.5f > std::fabsf(GetDoorAngle())); // Como el original: NaN devuelve true
}

// NOTSA
CVector CDoor::GetRotation() const {
    CVector rotation{ 0.f, 0.f, 0.f };
    rotation[m_axis] = m_angle;
    return rotation;
}

// NOTSA
void CDoor::UpdateFrameMatrix(CMatrix& mat) {
    mat.SetRotateKeepPos(GetRotation());
    mat.UpdateRW();
}
