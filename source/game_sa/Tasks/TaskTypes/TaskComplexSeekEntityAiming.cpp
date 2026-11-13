/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntityAiming
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

#include "TaskComplexSeekEntityAiming.h"

#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimpleUseGun.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x695C00 (lote 185A, sin validar; el listado decia CTaskComplexSeekEntityAiming): destructor con borrado de CTaskComplexSeekEntityAiming del EXE (ranura 0 de la vtable 0x870C94; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x694C10;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x694C10 es el cuerpo del destructor de la clase (vtable 0x870C94 en +0, limpia la referencia de m_ToSeek (+0xC) y termina en CTaskComplex::Destructor 0x61A3D0);
// aqui la envoltura Destructor() de la clase (cuerpo con hook, sin expansion en linea). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskComplexSeekEntityAiming* __fastcall SeekEntityAimingScalarDeletingDestructor(CTaskComplexSeekEntityAiming* self, void* /*edx*/, uint8 flags) {
    self->Destructor();
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexSeekEntityAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAiming, 0x870c94, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x694B90);
    RH_ScopedInstall(Destructor, 0x694C10);
    RH_ScopedNamedGlobalInstall(SeekEntityAimingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x695C00); // lote 185A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x695820);
    RH_ScopedVMTInstall(GetTaskType, 0x694C00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x695C20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x695D30);
    RH_ScopedVMTInstall(ControlSubTask, 0x694C70);
}

// 0x694B90
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(CEntity* toSeek, float seekRadius, float aimRadius) :
    m_ToSeek{toSeek},
    m_SeekRadius{seekRadius},
    m_AimRadius{aimRadius}
{
    CEntity::SafeRegisterRef(m_ToSeek);
}

// 0x695820
// Como el original: la copia recibe el radio de búsqueda (+0x10) y el de apuntado (+0x14)
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(const CTaskComplexSeekEntityAiming& o) :
    CTaskComplexSeekEntityAiming{o.m_ToSeek, o.m_SeekRadius, o.m_AimRadius}
{
}

// 0x694C10
CTaskComplexSeekEntityAiming::~CTaskComplexSeekEntityAiming() {
    CEntity::SafeCleanUpRef(m_ToSeek);
}

// 0x694C10 (lote 185A, sin validar; thiscall, sin argumentos de pila, `ret`, marco SEH que solo desenrolla y se omite): cuerpo del destructor del EXE. Orden del EXE: vtable de la clase (0x870C94), CEntity::CleanUpOldReference
// (0x571A00) sobre +0xC si no es nulo y CTaskComplex::Destructor (0x61A3D0, con hook). El destructor generado por el compilador dejaria la vtable de la DLL. Es el hook de 0x694C10 y lo llama el destructor con borrado (0x695C00).
CTaskComplexSeekEntityAiming* CTaskComplexSeekEntityAiming::Destructor() {
    *reinterpret_cast<uint32*>(this) = 0x870C94;
    CEntity::SafeCleanUpRef(m_ToSeek);
    CTaskComplexBody::Destructor(this, nullptr);
    return this;
}

// 0x695C20
CTask* CTaskComplexSeekEntityAiming::CreateNextSubTask(CPed* ped) {
    // Como el original: primero el tipo de la subtarea; tipos desconocidos devuelven nullptr
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        if (!m_ToSeek) {
            return nullptr;
        }
        return new CTaskComplexSeekEntity<>{
            m_ToSeek,
            50'000,
            1'000,
            m_SeekRadius,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_COMPLEX_SEEK_ENTITY:
        if (!m_ToSeek) {
            return nullptr;
        }
        return new CTaskSimpleStandStill{2'000};
    default:
        return nullptr;
    }
}

// 0x695D30
CTask* CTaskComplexSeekEntityAiming::CreateFirstSubTask(CPed* ped) {
    if (!m_ToSeek) {
        return nullptr;
    }
    return new CTaskComplexSeekEntity<>{
        m_ToSeek,
        50'000,
        1'000,
        m_SeekRadius,
        2.f,
        2.f,
        true,
        true
    };
}

// 0x694C70
CTask* CTaskComplexSeekEntityAiming::ControlSubTask(CPed* ped) {
    const auto pedToSoughtDist2DSq = (ped->GetPosition() - m_ToSeek->GetPosition()).SquaredMagnitude2D();
    // Como el original: la tarea secundaria de ataque se usa solo si es TASK_SIMPLE_USE_GUN; si hay otra no se crea ninguna
    auto& taskMgr = ped->GetTaskManager();
    CTask* const attackTask = taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
    const auto UseGun = [&]() -> CTaskSimpleUseGun* {
        return attackTask && attackTask->GetTaskType() == TASK_SIMPLE_USE_GUN ? static_cast<CTaskSimpleUseGun*>(attackTask) : nullptr;
    };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: {
        if (const auto pedUseGun = UseGun()) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        }
        if (pedToSoughtDist2DSq >= sq(m_SeekRadius)) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }
        break;
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        // Como el original: con NaN también se termina el apuntado
        if (!(pedToSoughtDist2DSq < sq(m_AimRadius))) {
            if (const auto pedUseGun = UseGun()) {
                pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::END_LEISURE);
            }
        } else if (!attackTask) {
            taskMgr.SetTaskSecondary(
                new CTaskSimpleUseGun{
                    m_ToSeek,
                    {},
                    eGunCommand::AIM,
                    1,
                    false
                },
                TASK_SECONDARY_ATTACK
            );
        } else if (const auto pedUseGun = UseGun()) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        }
        break;
    }
    default:
        break; // Como el original: otros tipos no hacen nada
    }
    return m_pSubTask;
}
