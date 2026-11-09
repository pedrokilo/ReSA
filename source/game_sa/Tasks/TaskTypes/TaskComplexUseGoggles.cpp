/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseGoggles
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseGoggles.h"

#include "TaskSimpleRunAnim.h"
#include "TaskSimplePlayerOnFoot.h"

// 0x634F30 (lote 138A, banco Fn138A): el EXE tiene su propia copia de la reenvoltura de CTaskComplex (`mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]`): pasa los tres argumentos
// a la ranura 6 (MakeAbortable) de la subtarea, sin comprobar que exista.
bool CTaskComplexUseGoggles::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x634F40
CTask* CTaskComplexUseGoggles::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GOGGLES_OFF:
        ped->TakeOffGoggles();
        return nullptr;
    case TASK_SIMPLE_GOGGLES_ON:
        ped->PutOnGoggles();
        ped->GetWeaponAE().AddAudioEvent(AE_WEAPON_FIRE);
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x634F90
CTask* CTaskComplexUseGoggles::CreateFirstSubTask(CPed* ped) {
    ped->GetPlayerData()->m_bDontAllowWeaponChange = true;

    const char* taskName = ped->m_pGogglesObject ? "GogglesOff" : "GogglesOn";
    const eTaskType taskType = ped->m_pGogglesObject ? TASK_SIMPLE_GOGGLES_OFF : TASK_SIMPLE_GOGGLES_ON;

    return new CTaskSimpleRunAnim(ANIM_GROUP_GOGGLES, ANIM_ID_FIRE, 4.0f, taskType, taskName, false);
}

// 0x635050
CTask* CTaskComplexUseGoggles::ControlSubTask(CPed* ped) {
    ped->GetPlayerData()->m_bDontAllowWeaponChange = true; // Como el original: se escribe antes de comprobar el puntero
    if (ped->GetPlayerData()) {
        ped->GetPlayerData()->m_bPlayerSprintDisabled = true;
        ped->GetPlayerData()->m_fTimeCanRun = std::max(ped->GetPlayerData()->m_fTimeCanRun, 0.0f);

        auto* task = ped->GetIntelligence()->FindTaskByType(TASK_SIMPLE_PLAYER_ON_FOOT);
        if (task)
            static_cast<CTaskSimplePlayerOnFoot*>(task)->PlayerControlZelda(ped->AsPlayer(), true);

        ped->GetPlayerData()->m_bPlayerSprintDisabled = false;
    }

    return m_pSubTask;
}

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x634EF0 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskComplexUseGoggles del EXE (thiscall, sin argumentos, `ret`, devuelve this). Base CTaskComplex (0x61A3B0) y vtable 0x86E4A8.
CTaskComplexUseGoggles* __fastcall UseGogglesConstructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) {
    CTaskComplexBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x86E4A8;
    return self;
}

// 0x634F20 (lote 138A, banco Fn138A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskComplexUseGoggles del EXE (`mov [ecx],0x86E4A8; jmp 0x61A3D0`). Deja la vtable de
// CTaskComplexUseGoggles del EXE (0x86E4A8) y sigue con el cuerpo de ~CTaskComplex (0x61A3D0, con hook).
void __fastcall UseGogglesDestructorBody(CTaskComplexUseGoggles* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E4A8;
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexUseGoggles::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseGoggles, 0x86E4A8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(UseGogglesConstructorBody, "Constructor", 0x634EF0);
    RH_ScopedNamedGlobalInstall(UseGogglesDestructorBody, "Destructor", 0x634F20);

    RH_ScopedVMTInstall(Clone, 0x637060);
    RH_ScopedVMTInstall(GetTaskType, 0x634F10);
    RH_ScopedVMTInstall(MakeAbortable, 0x634F30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634F40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x634F90);
    RH_ScopedVMTInstall(ControlSubTask, 0x635050);
};

void TaskComplexUseGogglesTestCode() {
    CStreaming::RequestModel(MODEL_NVGOGGLES, STREAMING_GAME_REQUIRED);
    CPlayerPed* player = FindPlayerPed();
    player->GiveWeapon(WEAPON_NIGHTVISION, 1, true);
}
