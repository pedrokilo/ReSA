/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexInWater
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexInWater.h"
#include "TaskSimpleSwim.h"
#include "TaskSimpleClimb.h"
#include "TaskComplexDie.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"

// 0x6396A0 (lote 207A, sin validar; el listado decia CTaskComplexUseClosestFreeScriptedAttractor): destructor con borrado de CTaskComplexInWater del EXE (ranura 0 de su vtable 0x86E4EC; thiscall, un argumento de pila, `ret 4`,
// 30 bytes: `push esi; mov esi,ecx; call 0x635100; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo es 0x635100 (CTaskComplexInWater::Destructor, con hook: el destructor
// de C++ de la clase). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede
// nombrar la ranura 0). Es amiga de la clase (en el espacio de nombres global) para poder llamar a Destructor, que es privado.
CTaskComplexInWater* __fastcall InWaterScalarDeletingDestructor(CTaskComplexInWater* self, void* /*edx*/, uint8 flags) {
    self->Destructor(); // 0x635100 (con hook): llamada al cuerpo enganchado, no al destructor expandido en linea
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

void CTaskComplexInWater::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexInWater, 0x86e4ec, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6350D0);
    RH_ScopedInstall(Destructor, 0x635100);
    RH_ScopedNamedGlobalInstall(InWaterScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6396A0); // lote 207A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x6370B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6350F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x635110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x635200);
    RH_ScopedVMTInstall(ControlSubTask, 0x635420);
}

CTaskComplexInWater::CTaskComplexInWater(const CTaskComplexInWater& o) :
    CTaskComplexInWater{}
{
}

// 0x635110
CTask* CTaskComplexInWater::CreateNextSubTask(CPed* ped) {
    if (const auto swim = notsa::dyn_cast_if_present<CTaskSimpleSwim>(m_pSubTask); swim && swim->m_pEntity) {
        ped->bIsInTheAir = true;
        return new CTaskSimpleClimb{
            swim->m_pEntity,
            swim->m_pClimbPos,
            swim->m_fAngle,
            swim->m_nSurfaceType,
            (swim->m_pClimbPos.z - ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab)
                ? eClimbHeights::CLIMB_PULLUP
                : eClimbHeights::CLIMB_GRAB,
            false
        };
    }
    return nullptr;
}

// 0x635200
CTask* CTaskComplexInWater::CreateFirstSubTask(CPed* ped) {
    if (ped->IsPlayer() || CPopulation::IsSunbather(ped)) {
        return new CTaskSimpleSwim{ nullptr, nullptr };
    }

    if (ped->bDrownsInWater) { // Como el original: `test [esi+470h], 100000h; je` (bit 20 = bDrownsInWater) -> muere ahogado
        return new CTaskComplexDie{
            WEAPON_DROWNING,
            ANIM_GROUP_DEFAULT,
            ANIM_ID_DROWN,
        };
    }

    if (const auto pedsGrp = ped->GetGroup()) {
        if (const auto leader = pedsGrp->GetMembership().GetLeader()) {
            if (leader->GetPlayerData()) { // Como el original: se comprueba m_pPlayerData (0x480), no el tipo de ped
                return new CTaskSimpleSwim{ nullptr, leader };
            }
        }
    }

    if (ped->IsCreatedByMission()) {
        // Como el original: CPedIntelligence::FindTaskByType (0x600EE0, solo tareas primarias), no FindActiveTaskByType.
        if (ped->GetIntelligence()->FindTaskByType(TASK_SIMPLE_GO_TO_POINT)) {
            // Como el original: se vuelve a llamar a FindTaskByType para leer el punto (0x635336)
            const auto goTo = static_cast<CTaskSimpleGoToPoint*>(ped->GetIntelligence()->FindTaskByType(TASK_SIMPLE_GO_TO_POINT));
            const CVector target = goTo->m_vecTargetPoint;
            return new CTaskSimpleSwim{ &target, nullptr };
        }
    }

    return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.f };
}
