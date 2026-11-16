#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskSimplePause : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_PAUSE;

    explicit CTaskSimplePause(int32 time = 0);
    CTaskSimplePause(const CTaskSimplePause&);
    ~CTaskSimplePause() = default;

    CTask* Clone() const override { return new CTaskSimplePause{*this}; }
    eTaskType GetTaskType() const override; // 0x48E800 (lote 149A, banco Fn149A: definido en el .cpp)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    friend class CTaskComplexSunbathe; // lote 197A: su CreateSubTask del EXE (0x638290) llama al cuerpo enganchado Constructor (0x48E750) y devuelve su EAX
    friend class CTaskComplexGoToPointAndStandStill; // lote 198A: su CreateSubTask (0x6682D0) tambien
    CTaskSimplePause* Constructor(int32 time);

private:
    CTaskTimer m_Timer; // Como el original: el constructor no escribe el relleno +0x12..+0x13
    int32      m_PauseIntervalMs;
};

VALIDATE_SIZE(CTaskSimplePause, 0x18);
