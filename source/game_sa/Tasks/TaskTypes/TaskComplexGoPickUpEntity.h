#pragma once

#include "TaskComplex.h"

class CEntity;

// Clase todavía no reconstruida: solo campos, sin funciones propias.
// Vtable 0x870B98 (11 entradas) verificado en gta_sa.exe:
//   [0] 0x6935F0 (dtor deleting)  [1] 0x692C80 Clone           [4] 0x691A40 GetTaskType
//   [8] 0x691AE0 CreateNextSubTask  [9] 0x693610 CreateFirstSubTask  [10] 0x691D50 ControlSubTask
class NOTSA_EXPORT_VTABLE CTaskComplexGoPickUpEntity : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_PICKUP_ENTITY;

    CEntity* m_pEntity;
    CVector  m_vecPosition;
    CVector  m_vecPickupPosition;
    uint32   m_nTimePassedSinceLastSubTaskCreatedInMs;
    int32    m_nAnimGroupId;
    bool     m_bAnimBlockReferenced;
    char     _pad[3];

private:
    friend void InjectHooksMain();
    static void InjectHooks() {
        RH_ScopedVirtualClass(CTaskComplexGoPickUpEntity, 0x870B98, 11);
        RH_ScopedCategory("Tasks/TaskTypes");

        // TODO: instalar las funciones conforme se reconstruyan.
    }
};

VALIDATE_SIZE(CTaskComplexGoPickUpEntity, 0x34);
