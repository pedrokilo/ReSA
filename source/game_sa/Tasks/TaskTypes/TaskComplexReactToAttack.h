#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE. Desde el lote 193A (sin validar) se registra en InjectHooksMain solo
// para enganchar funciones sueltas del EXE (sin vtable exportada); ver el .cpp.
class NOTSA_EXPORT_VTABLE CTaskComplexReactToAttack : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_REACT_TO_ATTACK;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x620B80
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // CTask* CreateNextSubTask(CPed* ped) override;
    // CTask* CreateFirstSubTask(CPed* ped) override;
    // CTask* ControlSubTask(CPed* ped) override;
};
