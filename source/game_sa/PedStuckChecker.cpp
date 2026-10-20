/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedStuckChecker
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedStuckChecker.h"

#include "Events/EventStuckInAir.h"
#include "Events/EventInWater.h"
#include "Events/EventGroup.h"
#include "General.h"
#include "Timer.h"

void CPedStuckChecker::InjectHooks() {
    RH_ScopedClass(CPedStuckChecker);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TestPedStuck, 0x602C00);
}

// 0x602C00
// Como el original (x87 a 24 bits: cada operacion se redondea a float; las comparaciones ordenadas dan falso con NaN).
// `m_radius` cuenta fotogramas desde el ultimo punto sin atasco; cuando el peaton lleva muchos fotogramas sin avanzar, o choca de lado contra un edificio, se declara
// atascado (`m_state` = 1) o se le recoloca (`m_state` = 2); con estado distinto de 0 se anade CEventStuckInAir salvo que haya un CEventInWater con aceleracion > 1.
bool CPedStuckChecker::TestPedStuck(CPed* ped, CEventGroup* eventGroup) {
    CVector   V{};      // desplazamiento desde el ultimo punto sin atasco
    CVector   A{};      // vector de trabajo (origen de la sonda vertical / direccion perpendicular)
    CColPoint colPoint1{}, colPoint2{};
    CEntity*  hitEntity{};
    float     counterF{}, f14{}, f10{};
    int32     side = 0;
    uint16    counter{};

    // 603459: sin colision, sujeto a otra entidad o muerto/muriendo: se reinicia
    if (!ped->m_bUsesCollision || ped->m_pAttachedTo != nullptr || ped->m_nPedState == PEDSTATE_DIE || ped->m_nPedState == PEDSTATE_DEAD) {
        m_radius = 0;
        m_state  = PED_STUCK_STATE_NONE;
        return false;
    }

    // 602F6E: de pie o estaba de pie
    if (ped->bIsStanding || ped->bWasStanding) {
        goto reset_counter;
    }
    // 602C76: salvo que haya recibido dano de una entidad con intensidad > 0, hace falta ser jugador y estar atascado (bIsStuck)
    if (!ped->m_pDamageEntity || !(ped->m_fDamageIntensity > 0.0f)) {
        if (!ped->IsPlayer()) {
            goto reset_counter;
        }
        if (!ped->m_bIsStuck) {
            goto reset_counter;
        }
    }
    // 602CAF: nadando, con jetpack o escalando no se comprueba
    if (ped->GetIntelligence()->GetTaskSwim()) {
        goto reset_counter;
    }
    if (ped->GetIntelligence()->GetTaskJetPack()) {
        goto reset_counter;
    }
    if (ped->GetIntelligence()->GetTaskClimb()) {
        goto reset_counter;
    }

    // 602CE8: contador de fotogramas y desplazamiento
    counter = m_radius;
    if (counter <= 10000 && counter != 0) {
        V        = ped->GetPosition() - m_lastNonStuckPoint;
        m_radius = static_cast<uint16>(counter + 1);
    } else {
        m_radius            = 1;
        m_state             = PED_STUCK_STATE_NONE;
        m_lastNonStuckPoint = ped->GetPosition();
    }

    {
        // 602D6C: tope inferior del paso de tiempo (0.01); NaN pasa tal cual
        const auto StepClamp = []() -> float {
            const float step = CTimer::ms_fTimeStep;
            return (0.01f > step) ? 0.01f : step;
        };
        counterF = static_cast<float>(static_cast<int32>(m_radius));   // fild + fstp: el contador ya actualizado
        {
            const float s     = StepClamp();
            const float limit = 50.0f / (4.0f * s);
            if (counterF > limit) {
                const float sq = V.SquaredMagnitude();
                if (counterF * 0.01f > sq) {
                    goto stuck_probe;
                }
            }
        }
        // 602DD6
        {
            const uint8 numCollided = ped->m_nNumEntitiesCollided;
            if (numCollided > 1) {
                const float s      = StepClamp();
                const float limit2 = 50.0f / (s + s);
                if (counterF > limit2) {
                    if (counterF * 0.004f > std::fabs(V.z)) {
                        goto stuck_probe;
                    }
                }
                goto check_state;
            }
            if (numCollided != 1) {
                goto check_state;
            }
            CEntity* collided = ped->m_apCollidedEntities[0];
            if (!collided || collided->GetType() != ENTITY_TYPE_BUILDING) {
                goto check_state;
            }
            const float s      = StepClamp();
            const float limit2 = 50.0f / (s + s);
            if (!(counterF > limit2)) {
                goto check_state;
            }
            if (!(counterF * 0.004f > std::fabs(V.z))) {
                goto check_state;
            }
            goto wall_probe;
        }
    }

stuck_probe:
    // 602E40: solo cada 8 fotogramas (segun el id del peaton); entre medias queda marcado como atascado
    if (((CTimer::m_FrameCounter + static_cast<uint8>(ped->m_nRandomSeed) + 3) & 7) != 0) {
        m_state = PED_STUCK_STATE_STUCK;
        goto check_state;
    }
    {
        A                = ped->GetPosition();
        A.z              = A.z + 1.0f;
        const float zEnd = ped->GetPosition().z - 1.0f;
        if (!CWorld::ProcessVerticalLine(A, zEnd, colPoint1, hitEntity, true, true, false, true, false, false, nullptr)) {
            goto check_state;
        }
        if (ped->bHeadStuckInCollision) {
            if (!(colPoint1.m_vecPoint.z + 1.0f < ped->GetPosition().z)) {
                goto set_standing;
            }
        }
        // 602EFA: con dano de un peaton y velocidad de impacto vertical alta no se recoloca
        if (ped->m_fDamageIntensity > 0.0f && ped->m_pDamageEntity && ped->m_pDamageEntity->GetType() == ENTITY_TYPE_PED
            && ped->m_vecLastCollisionImpactVelocity.z > 0.3f) {
            goto set_standing;
        }
        A.z = colPoint1.m_vecPoint.z + 1.0f;
        ped->SetPosn(A);
        if (ped->bHeadStuckInCollision) {
            ped->bHeadStuckInCollision = false;
        }
    }
set_standing:
    ped->bIsStanding = true;

reset_counter:
    m_radius = 0;
    m_state  = PED_STUCK_STATE_NONE;
    goto check_state;

wall_probe:
    // 603048: choque lateral contra un edificio: se sondea el suelo a ambos lados (perpendicular al desplazamiento)
    {
        A = CVector{ -V.y, V.x, 1.0f };
        A.Normalise();
        {
            const CVector origin = ped->GetPosition() + A;
            f14                  = 5001.0f;  // 0x459C4800
            if (CWorld::ProcessVerticalLine(origin, -20.0f, colPoint1, hitEntity, true, false, false, false, false, false, nullptr)) {
                const CVector& p      = ped->GetPosition();
                const CVector  target = p + A;
                if (CWorld::GetIsLineOfSightClear(p, target, true, true, false, true, false, false, false)) {
                    f14 = colPoint1.m_vecPoint.z;
                }
            }
        }
        {
            const CVector origin = ped->GetPosition() - A;
            f10                  = 5002.0f;  // 0x459C5000
            if (CWorld::ProcessVerticalLine(origin, -20.0f, colPoint2, hitEntity, true, false, false, false, false, false, nullptr)) {
                const CVector& p      = ped->GetPosition();
                const CVector  target = p - A;
                if (CWorld::GetIsLineOfSightClear(p, target, true, true, false, true, false, false, false)) {
                    f10 = colPoint2.m_vecPoint.z;
                }
            }
        }

        side = 0;
        if (f14 > 5000.0f && f10 > 5000.0f) {
            side = -1;
        } else {
            if ((ped->GetPosition().z - 1.0f < f14) && (f14 < 5000.0f) && ((f10 < 5001.0f) || (f14 < f10))) {
                side = 1;
            } else if ((ped->GetPosition().z - 1.0f < f10) && (f10 < 5001.0f)) {
                side = 2;
            }
            if (side != 0 && !ped->bHeadStuckInCollision) {
                const bool skipMove = ped->m_fDamageIntensity > 0.0f && ped->m_pDamageEntity && ped->m_pDamageEntity->GetType() == ENTITY_TYPE_PED
                                      && ped->m_vecLastCollisionImpactVelocity.z > 0.3f;
                if (!skipMove) {
                    ped->SetPosn(side == 2 ? colPoint2.m_vecPoint : colPoint1.m_vecPoint);
                    CVector& pos = ped->GetPosition();
                    pos.z        = pos.z + 1.0f;
                }
            }
        }

        // 6032D7: si el lado 1 queda mas bajo se invierte la direccion; fuerza hacia el lado elegido
        if (f14 < f10) {
            A.x = A.x * -1.0f;
            A.y = A.y * -1.0f;
            A.z = A.z * -1.0f;
        }
        A.z = 1.0f;
        ped->ApplyMoveForce(CVector{ A.x * 4.0f, A.y * 4.0f, 4.0f });
        if (side > -1) {
            const CVector delta{ A.x * 0.25f, A.y * 0.25f, A.z * 0.25f };
            ped->SetPosn(ped->GetPosition() + delta);
            const float angle       = CGeneral::GetRadianAngleBetweenPoints(A.x, A.y, 0.0f, 0.0f);
            ped->m_fCurrentRotation = angle;
            const float limited     = CGeneral::LimitRadianAngle(angle);
            ped->m_fCurrentRotation = limited;
            ped->m_fAimingRotation  = limited;
            ped->SetOrientation(0.0f, 0.0f, limited);
        }
        m_state = PED_STUCK_STATE_WAS_STUCK;
    }

check_state:
    // 602F76: sin estado de atasco no hay evento
    if (m_state == PED_STUCK_STATE_NONE) {
        return false;
    }
    // 602F80: un CEventInWater con aceleracion > 1 evita el evento (y baja la bandera de "de pie")
    if (eventGroup->GetEventOfType(EVENT_IN_WATER)) {
        const auto* water = static_cast<CEventInWater*>(eventGroup->GetEventOfType(EVENT_IN_WATER));
        if (water->m_acceleration > 1.0f) {
            ped->bIsStanding = false;
            return false;
        }
    }
    {
        CEventStuckInAir event(ped);
        eventGroup->Add(&event, false);
    }
    return true;
}
