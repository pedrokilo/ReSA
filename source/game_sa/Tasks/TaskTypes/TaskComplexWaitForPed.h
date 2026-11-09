#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForPed : public CTaskComplex
{
public:
    CPed* m_ped;
    float m_radius;
    uint32 m_timeInMs;
    bool m_bRotateOtherPedsToWaitingPed;
private:
    char padding[3];
    CTaskTimer m_timer;
    int32 m_framesToWaitForSettingRotation;

public:
    static void InjectHooks();
    CTaskComplexWaitForPed* Constructor(CPed* ped, float radius, uint32 time, bool rotate);

    //! 0x683950 (lote 132A, banco Fn132A): cuerpo del Clone de la clase del EXE (la clase aun no declara sus virtuales): reserva una tarea (0x2C bytes) y le aplica Constructor con los campos de esta
    CTask* CloneBody() const;
};

VALIDATE_SIZE(CTaskComplexWaitForPed, 0x2C);
