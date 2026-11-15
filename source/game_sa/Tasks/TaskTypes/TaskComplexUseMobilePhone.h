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

    // Lote 206A (sin validar): lectura directa del campo +8 (m_pSubTask) para las funciones sueltas enganchadas del EXE (CTaskComplex::GetSubTask es una llamada que el EXE no hace)
    CTask* GetSubTaskRaw() const { return m_pSubTask; }

    // Lote 206A (sin validar): crean las subtareas de CreateNextSubTask como el EXE: CTask::operator new (0x61A5A0) y, solo si hay memoria, el cuerpo enganchado del constructor (0x61A900 de CTaskSimpleRunAnim, 0x48E750 de CTaskSimplePause); nulo si el pool esta agotado
    static CTask* NewPhoneAnimTask(int32 animId, int32 taskType, uintptr_t exeNameAddress);
    static CTask* NewPhonePauseTask(int32 timeMs);
    // Lote 207A (sin validar). La clase sigue abstracta (faltan GetTaskType 0x6348D0, MakeAbortable 0x634930 y CreateNextSubTask 0x634A60); estas tres entran en la vtable.
    CTask* Clone() const override;                 // 0x636FD0
    CTask* CreateFirstSubTask(CPed* ped) override; // 0x634C10
    CTask* ControlSubTask(CPed* ped) override;     // 0x634D60
};

VALIDATE_SIZE(CTaskComplexUseMobilePhone, 0x20);
