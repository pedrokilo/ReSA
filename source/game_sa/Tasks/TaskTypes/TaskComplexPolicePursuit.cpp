/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPolicePursuit
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPolicePursuit.h"
#include "TaskComplexArrestPed.h"
#include "TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleScratchHead.h"

void CTaskComplexPolicePursuit::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPolicePursuit, 0x8709d4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68BA70);
    RH_ScopedInstall(Destructor, 0x68D880);

    RH_ScopedInstall(SetWeapon, 0x68BAD0);
    RH_ScopedInstall(ClearPursuit, 0x68BD90);
    RH_ScopedInstall(SetPursuit, 0x68BBD0);
    RH_ScopedInstall(PersistPursuit, 0x68BDC0);
    RH_ScopedInstall(CreateSubTask, 0x68D910);
    RH_ScopedVMTInstall(Clone, 0x68CDD0);
    RH_ScopedVMTInstall(GetTaskType, 0x68BAA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68BAB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68BAC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6908E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x690920);
}


// 0x68BA70 - Como el original: constructor proporcionado por el usuario (sin `{}` no hay puesta a cero previa
// del objeto; el EXE conserva los bits 3..7 del byte de banderas)
CTaskComplexPolicePursuit::CTaskComplexPolicePursuit() = default;

CTaskComplexPolicePursuit::CTaskComplexPolicePursuit(const CTaskComplexPolicePursuit& o) :
    CTaskComplexPolicePursuit{}
{
}

// 0x68D880
CTaskComplexPolicePursuit::~CTaskComplexPolicePursuit() {
    // Como el original: el perseguidor no tiene referencia registrada; solo se limpia la del perseguido (miembro)
    if (m_Pursuer) {
        ClearPursuit(m_Pursuer);
    }
}

// 0x68BAD0
// Make sure cop has a weapon on them
void CTaskComplexPolicePursuit::SetWeapon(CPed* ped) { // `ped` is the pursuer
    // Como el original: comparaciones con signo del nivel de busqueda
    const auto wantedLevel = (int32)FindPlayerWanted()->GetWantedLevel();

    if (wantedLevel > 1) {
        if (ped->GetActiveWeapon().GetType() != WEAPON_UNARMED) { // Already has a weapon
            return;
        }
    } else {
        if (wantedLevel != 1) {
            return;
        }
        if (FindPlayerPed()->m_standingOnEntity || ped->m_nPedState == PEDSTATE_ARREST_PLAYER) {
            return;
        }
        if (ped->DoWeHaveWeaponAvailable(WEAPON_NIGHTSTICK)) {
            // Como el original: datos del arma con habilidad estandar (1), pedidos dos veces
            if (CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetActiveWeapon().GetType(), eWeaponSkill::STD)->m_nWeaponFire != WEAPON_FIRE_INSTANT_HIT
                && CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetActiveWeapon().GetType(), eWeaponSkill::STD)->m_nWeaponFire != WEAPON_FIRE_PROJECTILE) {
                ped->SetCurrentWeapon(WEAPON_NIGHTSTICK);
                return;
            }
        }
    }

    ped->SetCurrentWeapon(ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN) ? WEAPON_SHOTGUN : WEAPON_PISTOL);
}

// 0x68BD90
void CTaskComplexPolicePursuit::ClearPursuit(CCopPed* pursuer) {
    if (FindPlayerPed()) {
        FindPlayerWanted()->RemovePursuitCop(pursuer);
    }
}

// 0x68BBD0
bool CTaskComplexPolicePursuit::SetPursuit(CPed* ped) {
    // Find closest player
    float minDistSq = FLT_MAX;
    CPlayerPed* closestPlayer{};
    for (const auto& v : CWorld::Players) {
        const auto plyr = v.m_pPed;
        if (!plyr) {
            continue;
        }
        const auto& plyrPos = plyr->GetPosition();
        const auto& pedPos  = ped->GetPosition();
        const float dx = plyrPos.x - pedPos.x, dy = plyrPos.y - pedPos.y, dz = plyrPos.z - pedPos.z;
        const float distSq = dz * dz + dx * dx + dy * dy; // Como el original: orden z, x, y
        if (!(distSq < minDistSq)) { // Como el original: tambien con NaN
            continue;
        }
        if (plyr->bInVehicle) {
            if (!(distSq * plyr->m_pVehicle->GetMoveSpeed().SquaredMagnitude() < sq(4.f))) { // TODO/BUG: Why `*`?
                continue;
            }
        }
        closestPlayer = plyr;
        minDistSq     = distSq;
    }

    // Como el original: se suelta la referencia anterior, se pide CWanted antes de comprobar y el perseguido
    // solo se guarda (con referencia) si SetPursuitCop acepta al policia
    m_Pursued = nullptr;
    const auto wanted = FindPlayerWanted();
    if (closestPlayer && wanted->SetPursuitCop(ped->AsCop())) {
        m_Pursued = closestPlayer;
        return true;
    }
    return false;
}

// 0x68BDC0
bool CTaskComplexPolicePursuit::PersistPursuit(CCopPed* pursuer) {
    const auto wanted = FindPlayerWanted();

    if (pursuer->m_fHealth <= 0.f) { // 0x68BDD0
        ClearPursuit(pursuer);
    } else if (CCullZones::NoPolice() && !m_IsRoadBlockCop) { // 0x68BDF1
        if (pursuer->bHitSomethingLastFrame) { // 0x68BE01
            m_IsPlayerInCullZone = m_IsRoadBlockCop = true;
            ClearPursuit(pursuer);
        }
    } else if (m_IsPlayerInCullZone && !CCullZones::NoPolice()) { // 0x68BE07 (Como el original: primero la bandera)
        m_IsPlayerInCullZone = m_IsRoadBlockCop = false;
        ClearPursuit(pursuer);
    } else if (wanted->GetWantedLevel() == eWantedLevel::WANTED_CLEAN) { // 0x68BE3C
        if (m_IsRoadBlockCop && !m_IsPlayerInCullZone) {
            m_IsPlayerInCullZone = m_IsRoadBlockCop = false;
            ClearPursuit(pursuer);
        }
    }

    wanted->RemoveExcessPursuitCops(); // 0x68BE5D
    return wanted->IsInPursuit(pursuer);
}

// 0x68D910
CTask* CTaskComplexPolicePursuit::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_ARREST_PED:
        // Como el original: sin perseguido no se crea nada
        if (!m_Pursued) {
            return nullptr;
        }
        return new CTaskComplexArrestPed{m_Pursued};
    case TASK_COMPLEX_SEEK_ENTITY:
        // Como el original: radio de la colision del modelo (CModelInfo) + 1
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
            ped->m_pVehicle,
            50'000,
            1'000,
            CModelInfo::GetModelInfo(ped->m_pVehicle->m_nModelIndex)->GetColModel()->GetBoundRadius() + 1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_SIMPLE_SCRATCH_HEAD:
        return new CTaskSimpleScratchHead{};
    default: // Como el original: cualquier otro tipo (incluido TASK_FINISHED) no crea nada
        return nullptr;
    }
}

// 0x6908E0
CTask* CTaskComplexPolicePursuit::CreateFirstSubTask(CPed* ped) {
    // Como el original: se guarda el perseguidor (sin registrar referencia)
    m_Pursuer = ped->AsCop();
    if (SetPursuit(ped)) {
        return CreateSubTask(TASK_COMPLEX_ARREST_PED, ped);
    }
    m_CouldJoinPursuit = false;
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x690920
CTask* CTaskComplexPolicePursuit::ControlSubTask(CPed* ped) {
    const auto nextSubTaskType = GetNextSubTaskType(ped->AsCop());

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_ARREST_PED || nextSubTaskType == TASK_COMPLEX_ARREST_PED) { // 0x690A3D
        SetWeapon(ped);
    }

    if (nextSubTaskType == TASK_NONE || !m_pSubTask->MakeAbortable(ped)) { // 0x690A56
        return m_pSubTask;
    }

    if (nextSubTaskType != TASK_COMPLEX_ENTER_CAR_AS_DRIVER) { // 0x690A6C
        return CreateSubTask(nextSubTaskType, ped);
    }

    // Como el original: el evento vive hasta despues de crear la tarea
    CEventVehicleToSteal vehicleToSteal{ ped->m_pVehicle };
    ped->GetEventGroup().Add(&vehicleToSteal, false);
    return new CTaskSimpleScratchHead{};
}

// 0x690956 (not a function originally)
eTaskType CTaskComplexPolicePursuit::GetNextSubTaskType(CCopPed* pursuer) { // ped is the pursuer
    const auto plyrWanted = FindPlayerWanted();

    if (PersistPursuit(pursuer)) { // 0x690956
        return TASK_NONE;
    }

    if (m_IsRoadBlockCop && (!pursuer->bStayInSamePlace || pursuer->GetActiveWeapon().GetType() != WEAPON_PISTOL)) { // 0x690991 (Inverted)
        pursuer->SetCurrentWeapon(WEAPON_PISTOL);
        pursuer->bStayInSamePlace = true;
        return TASK_COMPLEX_ARREST_PED;
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_ARREST_PED) { // 0x6909BB
        if (pursuer->bInVehicle) {
            return TASK_FINISHED;
        }

        if (!pursuer->m_pVehicle) {
            return TASK_SIMPLE_STAND_STILL;
        }

        if (plyrWanted->GetWantedLevel() == eWantedLevel::WANTED_CLEAN) { // 0x6909D7
            return TASK_COMPLEX_ENTER_CAR_AS_DRIVER;
        }

        // Como el original: distancia al cuadrado (0x406DA0) estrictamente menor que 25 (0x858FE8); si no, quedarse quieto
        if ((pursuer->m_pVehicle->GetPosition() - pursuer->GetPosition()).SquaredMagnitude() < 25.f) {
            return TASK_COMPLEX_SEEK_ENTITY;
        }
        return TASK_SIMPLE_STAND_STILL;
    }

    return TASK_NONE;
}
