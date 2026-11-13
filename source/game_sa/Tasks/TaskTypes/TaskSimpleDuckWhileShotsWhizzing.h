#pragma once

#include "TaskSimpleDuck.h"

class CPed;
class CEvent;

// Lote 187A (sin validar). Tarea de agacharse mientras pasan balas cerca: en el EXE es una clase derivada de CTaskSimpleDuck (vtable 0x870BEC, mismo tamano 0x28) cuyo constructor (0x692680) lleva en linea el de
// CTaskSimpleDuck con tipo de control 0 (DUCK_STANDALONE), la duracion recibida y contador -1, y despues deja su propia vtable. Sin campos propios.
class NOTSA_EXPORT_VTABLE CTaskSimpleDuckWhileShotsWhizzing : public CTaskSimpleDuck {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING;

    static void InjectHooks();

    explicit CTaskSimpleDuckWhileShotsWhizzing(uint16 lengthOfDuck);

    CTask* Clone() const override;                                                                                           // 0x692D60 (lote 187A, sin validar)
    eTaskType GetTaskType() const override;                                                                                  // 0x6926E0 (lote 187A, sin validar)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override; // 0x692700 (lote 187A, sin validar)
    bool ProcessPed(CPed* ped) override;                                                                                     // 0x694640 (lote 187A, sin validar)

private:
    friend void InjectHooksMain();

    static void __fastcall DestructorBody(CTaskSimpleDuckWhileShotsWhizzing* self, void* edx);                                         // 0x6926F0 (lote 187A, sin validar)
    static CTaskSimpleDuckWhileShotsWhizzing* __fastcall ScalarDeletingDestructor(CTaskSimpleDuckWhileShotsWhizzing* self, void* edx, uint8 flags); // 0x693B80 (lote 187A, sin validar)

    CTaskSimpleDuckWhileShotsWhizzing* Constructor(uint16 lengthOfDuck); // 0x692680 (lote 187A, sin validar)
};
VALIDATE_SIZE(CTaskSimpleDuckWhileShotsWhizzing, 0x28);
