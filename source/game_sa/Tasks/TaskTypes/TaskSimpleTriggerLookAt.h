/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "AnimBlendAssociation.h"
#include "Entity.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleTriggerLookAt : public CTaskSimple {
public:
    CEntity* m_pEntity;
    int32    m_time;
    int32    m_nOffsetBoneTag;
    RwV3d    m_vOffsetPos;
    bool     m_bUseTorso;
    float    m_Speed;
    int32    m_nBlendTime;
    bool     m_bEntityExist;
    int8     m_nPriority;

public:
    static constexpr auto Type = TASK_SIMPLE_TRIGGER_LOOK_AT;

    static void InjectHooks();

    CTaskSimpleTriggerLookAt(CEntity* entity,
                             int32 time,
                             int32 offsetBoneTag,
                             RwV3d offsetPos,
                             bool bUseTorso = true,
                             float speed = 0.25f,
                             int32 blendTime = 1000,
                             int32 priority = 3); // 0x634440 (lote 182A, sin validar)
    ~CTaskSimpleTriggerLookAt() override;        // cuerpo del EXE: 0x634500 (DestructorBody, lote 163A); el de la DLL solo libera la referencia

    CTask*    Clone() const override;                                                           // 0x634560 (lote 182A, sin validar)
    eTaskType GetTaskType() const override { return Type; }                                     // 0x6344F0 (lote 182A, sin validar)
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x634610 (lote 182A, sin validar)
    bool      ProcessPed(CPed* ped) override;                                                   // 0x634620 (lote 182A, sin validar)

private:
    // Envoltura para el hook del constructor (0x634440; thiscall, 10 dwords en la pila, `ret 0x28`, devuelve this)
    __declspec(noinline) CTaskSimpleTriggerLookAt* Constructor(CEntity* entity, int32 time, int32 offsetBoneTag, RwV3d offsetPos, bool bUseTorso, float speed, int32 blendTime, int32 priority) {
        this->CTaskSimpleTriggerLookAt::CTaskSimpleTriggerLookAt(entity, time, offsetBoneTag, offsetPos, bUseTorso, speed, blendTime, priority);
        return this;
    }
};

VALIDATE_SIZE(CTaskSimpleTriggerLookAt, 0x30);
