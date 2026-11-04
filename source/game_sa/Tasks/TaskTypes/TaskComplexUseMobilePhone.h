/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseMobilePhone : public CTaskComplex {
public:
    int32      m_nDuration;
    CTaskTimer m_timer;
    bool       m_bIsAborting;
    bool       m_bQuit;

public:
    static constexpr auto Type = TASK_COMPLEX_USE_MOBILE_PHONE;

    static void InjectHooks();

    CTaskComplexUseMobilePhone(int32 nDuration);

    // 0x634A40: pide terminar la llamada (marca m_bQuit y aborta la tarea con prioridad ABORT_PRIORITY_LEISURE)
    void Quit(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexUseMobilePhone, 0x20);
