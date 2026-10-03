/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventKnockOffBike
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventKnockOffBike.h"

#include "TaskComplexEnterCar.h"
#include "TaskSimpleCarSetPedOut.h"

void CEventKnockOffBike::InjectHooks()
{
    RH_ScopedVirtualClass(CEventKnockOffBike, 0x85B2E0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedOverloadedInstall(Constructor, "first", 0x4AFCF0, CEventKnockOffBike*(CEventKnockOffBike::*)(CVehicle*, CVector*, CVector*, float, float, uint8, uint8, int32, CPed*, bool, bool));
    RH_ScopedOverloadedInstall(Constructor, "second", 0x4AFC70, CEventKnockOffBike*(CEventKnockOffBike::*)());
    RH_ScopedVMTInstall(AffectsPed, 0x4AFEE0);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B4E80);
    RH_ScopedInstall(From, 0x4AFDD0);
    RH_ScopedInstall(SetPedOutCar, 0x4AFF60);
    RH_ScopedInstall(CalcForcesAndAnims, 0x4B0020);
    RH_ScopedInstall(SetPedSafePosition, 0x4B4AC0);
}

CEventKnockOffBike::CEventKnockOffBike(CVehicle* vehicle, const CVector& moveSpeed, const CVector& collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff)
{
    m_moveSpeed = moveSpeed;
    m_collisionImpactVelocity = collisionImpactVelocity;
    m_time = time;
    m_damageIntensity = damageIntensity;
    field_28 = a6;
    m_ped = ped;
    m_knockOffDirection = knockOffDirection;
    m_isVictimDriver = isVictimDriver;
    m_forceKnockOff = forceKnockOff;
    m_knockOffType = knockOffType;
    m_exitDoor = 0;
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_ped);
}

CEventKnockOffBike::CEventKnockOffBike()
{
    m_damageIntensity = 0.0f;
    field_28 = 0.0f;
    m_time = 0;
    m_ped = nullptr;
    m_knockOffDirection = 0;
    m_isVictimDriver = false; // Como el original: solo se borra el bit 0 de +0x35 (m_forceKnockOff no se toca)
    m_knockOffType = 0;
    m_exitDoor = 0;
    m_vehicle = nullptr;
}

CEventKnockOffBike::~CEventKnockOffBike()
{
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_ped);
}

CEventKnockOffBike* CEventKnockOffBike::Constructor(CVehicle* vehicle, CVector* moveSpeed, CVector* collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff)
{
    this->CEventKnockOffBike::CEventKnockOffBike(vehicle, *moveSpeed, *collisionImpactVelocity, damageIntensity, a6, knockOffType, knockOffDirection, time, ped, isVictimDriver, forceKnockOff);
    return this;
}

CEventKnockOffBike* CEventKnockOffBike::Constructor()
{
    this->CEventKnockOffBike::CEventKnockOffBike();
    return this;
}

// 0x4AFEE0
bool CEventKnockOffBike::AffectsPed(CPed* ped)
{
    if (ped->IsAlive()) {
        if (m_vehicle && m_vehicle->GetStatus() == STATUS_GHOST)
            return false;
        // Como el original: (flags & 0x18000000) == 0x08000000, es decir CantBeKnockedOffBike == NEVER
        if (ped->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_NEVER && !m_forceKnockOff)
            return false;
        if (m_vehicle) {
            if (m_vehicle->m_pDriver != ped
                && m_vehicle->m_apPassengers[0] != ped
                && m_vehicle->m_apPassengers[1] != ped
                && m_vehicle->m_apPassengers[2] != ped)
            {
                return false;
            }
            if (m_vehicle->m_pDriver == ped)
                m_isVictimDriver = true;
            return true;
        }
    }
    return false;
}

// 0x4B4E80
void CEventKnockOffBike::ReportCriminalEvent(CPed* ped)
{
    if (IsCriminalEvent()) {
        if (ped->m_nPedType == PED_TYPE_COP && m_ped && m_ped->IsPlayer())
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_DAMAGE_COP_CAR, ped->GetPosition(), (uint32)ped, false);
    }
}

// 0x4AFDD0
void CEventKnockOffBike::From(const CEventKnockOffBike& right)
{
    m_vehicle = right.m_vehicle;
    m_moveSpeed = right.m_moveSpeed;
    m_collisionImpactVelocity = right.m_collisionImpactVelocity;
    m_damageIntensity = right.m_damageIntensity;
    field_28 = right.field_28;
    m_knockOffType = right.m_knockOffType;
    m_knockOffDirection = right.m_knockOffDirection;
    m_time = right.m_time;
    m_ped = right.m_ped;
    m_isVictimDriver = right.m_isVictimDriver; // Como el original: solo se copia el bit 0 de +0x35
    m_exitDoor = right.m_exitDoor;
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AFF60
void CEventKnockOffBike::SetPedOutCar(CPed* ped)
{
    m_exitDoor = TARGET_DOOR_DRIVER;
    if (m_vehicle->m_pDriver == ped)
        m_exitDoor = TARGET_DOOR_DRIVER;
    else if (m_vehicle->IsBike())
        m_exitDoor = TARGET_DOOR_REAR_LEFT;
    else if (m_vehicle->m_apPassengers[0] == ped)
        m_exitDoor = TARGET_DOOR_FRONT_RIGHT;
    else if (m_vehicle->m_apPassengers[1] == ped)
        m_exitDoor = TARGET_DOOR_REAR_LEFT;
    else if (m_vehicle->m_apPassengers[2] == ped)
        m_exitDoor = TARGET_DOOR_REAR_RIGHT;
    CTaskSimpleCarSetPedOut taskCarSetPedOut(m_vehicle, (eTargetDoor)m_exitDoor, true);
    taskCarSetPedOut.m_bKnockedOffBike = true;
    taskCarSetPedOut.m_bSwitchOffEngine = false;
    taskCarSetPedOut.ProcessPed(ped);
    CCarEnterExit::RemoveCarSitAnim(ped);
}

namespace {
// Como el original: productos y sumas en el x87 (sin vaciar desnormales y con el exponente extendido hasta el fstp)
float X87Mul(float a, float b) {
    float r;
    __asm {
        fld a
        fmul b
        fstp r
    }
    return r;
}

float X87Mul4(float a, float b, float c, float d) {
    float r;
    __asm {
        fld a
        fmul b
        fmul c
        fmul d
        fstp r
    }
    return r;
}

// a * ca + b * cb
float X87MulAdd(float a, float ca, float b, float cb) {
    float r;
    __asm {
        fld a
        fmul ca
        fld b
        fmul cb
        faddp st(1), st
        fstp r
    }
    return r;
}

CVector X87Scale(const CVector& v, float k) {
    return { X87Mul(v.x, k), X87Mul(v.y, k), X87Mul(v.z, k) };
}

CVector X87Combine(const CVector& a, float ca, const CVector& b, float cb) {
    return { X87MulAdd(a.x, ca, b.x, cb), X87MulAdd(a.y, ca, b.y, cb), X87MulAdd(a.z, ca, b.z, cb) };
}
} // namespace

// 0x4B0020
int32 CEventKnockOffBike::CalcForcesAndAnims(CPed* ped)
{
    // Como el original: animación 15 cuando la dirección no está en [0, 3] (ebp inicial)
    constexpr int32 DEFAULT_ANIM = 15;

    uint8 numContactWheels = 0;
    float massRatio;
    {
        const float pedMass = ped->m_fMass, vehMass = m_vehicle->m_fMass;
        __asm {
            fld pedMass
            fdiv vehMass
            fstp massRatio
        }
    }
    if (m_vehicle->IsBike())
        numContactWheels = m_vehicle->AsBike()->m_nNoOfContactWheels;
    else if (m_vehicle->IsAutomobile())
        numContactWheels = m_vehicle->AsAutomobile()->m_nNumContactWheels;

    // Como el original: la dirección se lee con signo
    const auto direction = static_cast<int8>(m_knockOffDirection);

    switch (m_knockOffType)
    {
    case KNOCK_OFF_TYPE_NONE:
    case KNOCK_OFF_TYPE_NONE_1:
        ped->m_vecMoveSpeed = m_moveSpeed;
        ped->m_pEntityIgnoredCollision = m_vehicle;
        return ANIM_ID_NO_ANIMATION_SET;
    default:
    {
        // Como el original: (x² + y²) + z² del vehículo; con NaN se toma la rama lenta en 1..3 y no se aplica fuerza en 0
        const auto& vms = m_vehicle->m_vecMoveSpeed;
        const float speedSq = vms.x * vms.x + vms.y * vms.y + vms.z * vms.z;
        switch (direction)
        {
        case 0:
        {
            if (speedSq < 0.09f) {
                CVector force = X87Combine(m_vehicle->m_matrix->GetForward(), -6.0f, m_vehicle->m_matrix->GetUp(), 5.0f);
                ped->ApplyMoveForce(force);
            }
            ped->m_pEntityIgnoredCollision = m_vehicle;
            return ANIM_ID_BIKE_FALLR;
        }
        case 1:
        case 2:
        {
            if (!(speedSq > 0.09f)) {
                ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed;
                CVector force = X87Combine(m_vehicle->m_matrix->GetUp(), 4.0f, m_vehicle->m_matrix->GetRight(), 8.0f);
                ped->ApplyMoveForce(force);
                return ANIM_ID_KD_LEFT;
            }
            ped->m_vecMoveSpeed = X87Scale(m_vehicle->m_vecMoveSpeed, 0.3f);
            CVector force = X87Combine(m_vehicle->m_matrix->GetUp(), 5.0f, m_vehicle->m_matrix->GetRight(), 6.0f);
            ped->ApplyMoveForce(force);
            return ANIM_ID_KO_SPIN_R;
        }
        case 3:
        {
            if (!(speedSq > 0.09f)) {
                ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed;
                CVector force = X87Combine(m_vehicle->m_matrix->GetUp(), 4.0f, m_vehicle->m_matrix->GetRight(), -8.0f);
                ped->ApplyMoveForce(force);
                return ANIM_ID_KD_RIGHT;
            }
            ped->m_vecMoveSpeed = X87Scale(m_vehicle->m_vecMoveSpeed, 0.3f);
            CVector force = X87Combine(m_vehicle->m_matrix->GetUp(), 5.0f, m_vehicle->m_matrix->GetRight(), -6.0f);
            ped->ApplyMoveForce(force);
            return ANIM_ID_KO_SPIN_L;
        }
        default:
            return DEFAULT_ANIM; // Como el original: sin efectos
        }
    }
    case KNOCK_OFF_TYPE_SKIDBACKFRONT:
    {
        ped->m_vecMoveSpeed = m_moveSpeed;
        float factor = -0.3f;
        if (m_vehicle->IsSubBMX())
            factor = -0.2f;
        // Como el original: ApplyMoveForce(x, y, 0)
        ped->ApplyMoveForce(
            X87Mul4(factor, m_collisionImpactVelocity.x, massRatio, m_damageIntensity),
            X87Mul4(factor, m_collisionImpactVelocity.y, massRatio, m_damageIntensity),
            0.0f
        );
        float randomNum = 5.0f;
        if (m_vehicle->IsSubBMX())
            randomNum = 6.0f;
        // Como el original: se multiplica por 1/70 (0x85B328) en lugar de dividir entre 70
        ped->m_vecMoveSpeed.z += CGeneral::GetRandomNumberInRange(3.0f, randomNum) * std::bit_cast<float>(0x3C6A0EA1u);
        ped->m_pEntityIgnoredCollision = m_vehicle;
        int32 animId = DEFAULT_ANIM;
        switch (direction)
        {
        case 0:
            animId = ANIM_ID_KO_SKID_BACK;
            break;
        case 1:
            animId = ANIM_ID_KD_RIGHT;
            break;
        case 2:
            animId = ANIM_ID_KO_SKID_FRONT;
            break;
        case 3:
            animId = ANIM_ID_KD_LEFT;
            break;
        }
        ped->bKnockedOffBike = true;
        ped->IsPlayer(); // leftover?
        return animId;
    }
    case KNOCK_OFF_TYPE_FALL:
    {
        ped->m_vecMoveSpeed = X87Scale(m_vehicle->m_vecMoveSpeed, 0.2f);
        ped->m_vecMoveSpeed.z = 0.0f;
        ped->m_pEntityIgnoredCollision = m_vehicle;
        return ANIM_ID_FALL_FALL;
    }
    case KNOCK_OFF_TYPE_SKIDBACK_FALLR:
    {
        ped->m_vecMoveSpeed = m_moveSpeed;
        // Como el original: el número aleatorio se pide antes de calcular x e y
        const float forceZ = CGeneral::GetRandomNumberInRange(3.0f, 5.0f);
        ped->ApplyMoveForce(
            X87Mul4(-0.3f, m_collisionImpactVelocity.x, massRatio, m_damageIntensity),
            X87Mul4(-0.3f, m_collisionImpactVelocity.y, massRatio, m_damageIntensity),
            forceZ
        );
        ped->m_pEntityIgnoredCollision = m_vehicle;
        int32 animId = DEFAULT_ANIM;
        switch (direction)
        {
        case 0:
            animId = ANIM_ID_KO_SKID_BACK;
            break;
        case 1:
            animId = ANIM_ID_KD_RIGHT;
            break;
        case 2:
            animId = ANIM_ID_BIKE_FALLR;
            break;
        case 3:
            animId = ANIM_ID_KD_LEFT;
            break;
        }
        if (!numContactWheels)
            ped->bKnockedOffBike = true;
        return animId;
    }
    }
}

// 0x4B4AC0
bool CEventKnockOffBike::SetPedSafePosition(CPed* ped)
{
    if (m_vehicle->IsBike()) {
        CBike* bike = m_vehicle->AsBike();
        bike->m_RideAnimData.LeanAngle = 0.0f;
        bike->m_bLeanMatrixCalculated = false;
        ped->SetPedPositionInCar();
    }

    // Como el original: GetHeading en línea con fpatan (precisión completa); boca abajo (up.z < 0, NaN no) se suma
    // PI (0x858CB8) al resultado sin redondear antes de LimitRadianAngle. La matriz del vehículo se lee sin comprobarla.
    {
        CMatrix* const vehMat = m_vehicle->m_matrix;
        const float upZ = vehMat->GetUp().z;
        float heading;
        if (vehMat) {
            const float fwdX = vehMat->GetForward().x, fwdY = vehMat->GetForward().y;
            if (upZ < 0.0f) {
                const float pi = std::bit_cast<float>(0x40490FDBu);
                float sum;
                __asm {
                    fld fwdX
                    fchs
                    fld fwdY
                    fpatan
                    fadd pi
                    fstp sum
                }
                heading = CGeneral::LimitRadianAngle(sum);
            } else {
                __asm {
                    fld fwdX
                    fchs
                    fld fwdY
                    fpatan
                    fstp heading
                }
            }
        } else {
            heading = m_vehicle->m_placement.m_fHeading;
            if (upZ < 0.0f) {
                heading = CGeneral::LimitRadianAngle(heading + std::bit_cast<float>(0x40490FDBu));
            }
        }
        ped->m_fAimingRotation = heading;
    }

    ped->m_fCurrentRotation = ped->m_fAimingRotation;
    ped->SetHeading(ped->m_fAimingRotation);

    if (m_vehicle->IsBike() && !m_isVictimDriver) {
        // Como el original: producto escalar (z·z' + y·y') + x·x' con las matrices leídas directamente
        const auto& vf = m_vehicle->m_matrix->GetForward();
        const auto& pf = ped->m_matrix->GetForward();
        float forwardDistance = (1.0f - std::fabs(vf.z * pf.z + vf.y * pf.y + vf.x * pf.x)) * 0.8f;
        CVector distance = ped->GetPosition() - (forwardDistance * pf);
        ped->SetPosn(distance);
    }

    bool isSafe = true;
    bool findClosestNode = false;
    if (ped->m_fHealth <= 0.0f || m_ped) {
        findClosestNode = true;
    }

    CWorld::pIgnoreEntity = m_vehicle;
    ped->m_pEntityIgnoredCollision = m_vehicle;
    CVector savedPedPos = ped->GetPosition();
    bool knockPedOffBike = false;
    if (!ped->TestCollision(false) &&
        CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false)) {
        if (m_knockOffType == KNOCK_OFF_TYPE_SKIDBACKFRONT) {
            ped->GetPosition().z += 0.5f;
            // Como el original: si hay colisión se restaura la posición sin volver a mirar la línea de visión
            if (ped->TestCollision(false))
                ped->SetPosn(savedPedPos);
            else if (!CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false))
                ped->SetPosn(savedPedPos);
        }
        knockPedOffBike = true;
    }
    else {
        ped->GetPosition().z += 0.6f;
        if (!ped->TestCollision(false) &&
            CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false)) {
            ped->m_pEntityIgnoredCollision = m_vehicle;
            knockPedOffBike = true;
        }
        else {
            ped->SetPosn(savedPedPos);
            ped->GetPosition().z -= 0.6f;
            if (!ped->TestCollision(false) &&
                CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false)) {
                ped->m_pEntityIgnoredCollision = m_vehicle;
                knockPedOffBike = true;
            }
            else {
                // Como el original: la puerta de salida se extiende con signo
                isSafe = ped->PositionPedOutOfCollision(static_cast<int8>(m_exitDoor), m_vehicle, findClosestNode);
                if (isSafe) {
                    ped->m_pEntityIgnoredCollision = m_vehicle;
                    knockPedOffBike = true;
                }
            }
        }
    }

    if (knockPedOffBike) {
        ped->bKnockedOffBike = true;
        CEntity::ClearReference(ped->m_standingOnEntity);
        ped->bWasStanding = false;
        ped->bIsStanding = false;
    }

    CWorld::pIgnoreEntity = nullptr;
    return isSafe;
}
