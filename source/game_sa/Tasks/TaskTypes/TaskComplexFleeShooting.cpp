/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFleeShooting
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
#include "TaskComplexFleeShooting.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGunControl.h"

namespace {
// 0x65C9A0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexFleeShooting del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x65C9A0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexFleeShootingDestructorBody(CTaskComplexFleeShooting* self, void* /*edx*/) { self->CTaskComplexFleeShooting::~CTaskComplexFleeShooting(); }

// 0x65D2A0 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexFleeShooting del EXE (ranura 0 de su vtable 0x86F7C4; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x65C9A0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexFleeShooting* __fastcall CTaskComplexFleeShootingScalarDeletingDestructor(CTaskComplexFleeShooting* self, void* /*edx*/, uint8 flags) {
    CTaskComplexFleeShootingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexFleeShooting() { return reinterpret_cast<uintptr_t>(&CTaskComplexFleeShootingDestructorBody); } // para el banco Fn157A

void CTaskComplexFleeShooting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeShooting, 0x86f7c4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexFleeShootingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65D2A0); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x65C900);
    RH_ScopedInstall(Destructor, 0x65C9A0);

    RH_ScopedVMTInstall(Clone, 0x65CFB0);
    RH_ScopedVMTInstall(GetTaskType, 0x65C990);
    RH_ScopedVMTInstall(MakeAbortable, 0x65CA00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65CA60);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65CA70);
    RH_ScopedVMTInstall(ControlSubTask, 0x65CA90);
}

// 0x65C900
CTaskComplexFleeShooting::CTaskComplexFleeShooting(
    CEntity* entity,
    bool pedScream,
    float safeDist,
    int32 fleeTime,
    int32 shootTime,
    int32 shootRecoverTime,
    int32 posCheckPeriod,
    float posChangeTolerance
) :
    CTaskComplexSmartFleeEntity{ entity, pedScream, safeDist, fleeTime, posCheckPeriod, posChangeTolerance},
    m_shootRecoverTime{shootRecoverTime},
    m_shootTime{shootTime}
{
    // Como el original: la entidad se registra otra vez (ya la registra SmartFlee; el segundo registro no hace nada)
    CEntity::SafeRegisterRef(m_fleeFrom);
}

CTaskComplexFleeShooting::CTaskComplexFleeShooting(const CTaskComplexFleeShooting& o) :
    CTaskComplexFleeShooting{ o.m_fleeFrom, o.m_pedScream, o.m_safeDistance, o.m_time, o.m_shootTime, o.m_shootRecoverTime, o.m_posCheckPeriod, o.m_posChangeTolerance }
{
}

// 0x65C9A0
CTaskComplexFleeShooting::~CTaskComplexFleeShooting() {
    // Como el original: la entidad se limpia aquí y otra vez en el destructor de SmartFlee
    CEntity::SafeCleanUpRef(m_fleeFrom);
}

// 0x65CA00
bool CTaskComplexFleeShooting::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (const auto useGun = notsa::dyn_cast_if_present<CTaskSimpleUseGun>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK))) {
        useGun->MakeAbortable(ped, priority, event);
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65CA70
CTask* CTaskComplexFleeShooting::CreateFirstSubTask(CPed* ped) {
    m_gunTimer.SetAsOutOfTime();
    return CTaskComplexSmartFleeEntity::CreateFirstSubTask(ped);
}

// 0x65CA90
CTask* CTaskComplexFleeShooting::ControlSubTask(CPed* ped) {
    if (const auto attack = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        m_moveState = PEDMOVE_RUN; // Como el original: con tarea de ataque se huye corriendo
        if (const auto useGun = notsa::dyn_cast_if_present<CTaskSimpleUseGun>(attack)) {
            if (m_gunTimer.IsOutOfTime()) {
                if (useGun->GetLastGunCommand() == eGunCommand::FIREBURST) {
                    // Como el original: el temporizador se arranca sin comprobar el periodo
                    m_gunTimer.m_nStartTime = CTimer::GetTimeInMS();
                    m_gunTimer.m_nInterval = m_shootRecoverTime;
                    m_gunTimer.m_bStarted = true;

                    useGun->ControlGun(ped, m_fleeFrom, eGunCommand::END_LEISURE);
                }
            } else {
                useGun->ControlGun(ped, m_fleeFrom, eGunCommand::FIREBURST);
            }
        }
    } else {
        m_moveState = PEDMOVE_SPRINT;
        if (m_gunTimer.IsOutOfTime()) {
            // Como el original: arma activa con índice con signo y CPed::GetWeaponSkill() sin argumentos
            const auto weaponType = ped->m_aWeapons.data()[(int8)ped->m_nActiveWeaponSlot].m_Type;
            if (CWeaponInfo::GetWeaponInfo(weaponType, ped->GetWeaponSkill())->flags.bAimWithArm) {
                // Como el original: el temporizador se arranca sin comprobar el periodo
                m_gunTimer.m_nStartTime = CTimer::GetTimeInMS();
                m_gunTimer.m_nInterval = m_shootTime;
                m_gunTimer.m_bStarted = true;

                ped->GetTaskManager().SetTaskSecondary(
                    new CTaskSimpleUseGun{ m_fleeFrom, {}, eGunCommand::FIREBURST, 3 },
                    TASK_SECONDARY_ATTACK
                );
            }
        }
    }

    return CTaskComplexSmartFleeEntity::ControlSubTask(ped);
}
