#pragma once

#include "TaskTimer.h"
#include "TaskComplexSmartFleeEntity.h"

class CEvent;
class CEntity;
class CPed;
class CTaskComplexFleeShooting;

class NOTSA_EXPORT_VTABLE CTaskComplexFleeShooting : public CTaskComplexSmartFleeEntity {

public:
    int32       m_shootTime{};
    int32       m_shootRecoverTime{};
    CTaskTimer  m_gunTimer; // Como el original: relleno 0x52..0x53 sin escribir

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FLEE_SHOOTING;

    CTaskComplexFleeShooting(CEntity* entityToFleeFrom, bool pedScream, float safeDist, int32 fleeTime, int32 shootTime, int32 shootRecoverTime, int32 entityPosCheckInterval, float entityPosChangeThreshold);
    CTaskComplexFleeShooting(const CTaskComplexFleeShooting&);
    ~CTaskComplexFleeShooting();

    CTask*    Clone() const override { return new CTaskComplexFleeShooting{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    // 0x65CA60 - Como el original: salta a CTaskComplexSmartFleeEntity::CreateNextSubTask (0x65C680). Función propia para que su hook
    // no comparta dirección del DLL con el de SmartFlee (si no, al desactivar uno el DLL salta al EXE y el EXE vuelve al DLL sin fin)
    CTask*    CreateNextSubTask(CPed* ped) override { return CTaskComplexSmartFleeEntity::CreateNextSubTask(ped); }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override; 
private: // Wrappers for hooks
    // 0x65C900
    CTaskComplexFleeShooting* Constructor(CEntity* entityToFleeFrom, bool pedScream, float safeDist, int32 fleeTime, int32 shootTime, int32 shootRecoverTime,
                                          int32 entityPosCheckInterval, float entityPosChangeThreshold) {
        this->CTaskComplexFleeShooting::CTaskComplexFleeShooting(entityToFleeFrom, pedScream, safeDist, fleeTime, shootTime, shootRecoverTime, entityPosCheckInterval,
                                                                 entityPosChangeThreshold);
        return this;
    }
    // 0x65C9A0
    CTaskComplexFleeShooting* Destructor() {
        this->CTaskComplexFleeShooting::~CTaskComplexFleeShooting();
        return this;
    }
};
