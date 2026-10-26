#pragma once

#include "TaskComplex.h"

class CEntity;
class CPed;

// Vtable 0x870B98 (11 entradas) verificado en gta_sa.exe:
//   [0] 0x6935F0 (dtor deleting, llama a 0x691A50)  [1] 0x692C80 Clone  [4] 0x691A40 GetTaskType (0x136)
//   [6] 0x4211B0 MakeAbortable de CTaskComplex  [7] 0x61A430 SetSubTask de CTaskComplex
//   [8] 0x691AE0 CreateNextSubTask  [9] 0x693610 CreateFirstSubTask  [10] 0x691D50 ControlSubTask
// Reconstruidos: constructor (0x6919C0), destructor (0x691A50), Clone, GetTaskType y ControlSubTask (0x691D50, virtual). CreateNextSubTask y CreateFirstSubTask siguen en el EXE (reenvios).
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

public:
    CTaskComplexGoPickUpEntity(CEntity* entity, int32 animGroupId); // 0x6919C0
    ~CTaskComplexGoPickUpEntity() override;                         // 0x691A50

    CTask*    Clone() const override;                               // 0x692C80
    eTaskType GetTaskType() const override { return Type; }        // 0x691A40
    CTask*    CreateNextSubTask(CPed* ped) override;                // 0x691AE0
    CTask*    CreateFirstSubTask(CPed* ped) override;               // 0x693610
    CTask*    ControlSubTask(CPed* ped) override;                   // 0x691D50

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexGoPickUpEntity* Constructor(CEntity* entity, int32 animGroupId) { this->CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(entity, animGroupId); return this; }
    CTaskComplexGoPickUpEntity* Destructor() { this->CTaskComplexGoPickUpEntity::~CTaskComplexGoPickUpEntity(); return this; }
};

VALIDATE_SIZE(CTaskComplexGoPickUpEntity, 0x34);
