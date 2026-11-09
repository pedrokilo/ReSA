#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CTaskComplexWaitForBackup;
class CPed;
class CTaskSimpleGunControl;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForBackup : public CTaskComplex {
public:
    float      m_waitDistance{};
    CPed*      m_aggressor{};
    CPed*      m_backup{};
    CTaskTimer m_timer; // Como el original: sin inicialización por valor (el relleno tras los bool no se escribe)

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_BACKUP;

    CTaskComplexWaitForBackup(CPed* attacker, CPed* backup, float waitDist);
    CTaskComplexWaitForBackup(const CTaskComplexWaitForBackup&);
    ~CTaskComplexWaitForBackup();

    CTask* CreateSubTask(eTaskType taskType, CPed* a3);

    CTask*    Clone() const override { return new CTaskComplexWaitForBackup{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x697290
    CTaskComplexWaitForBackup* Constructor(CPed* aggressor, CPed* backup, float waitDist) {
        this->CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(aggressor, backup, waitDist);
        return this;
    }
    // 0x697330 (sin expansion en linea: la llama ScalarDeletingDestructor y el banco Fn142A la sustituye)
    __declspec(noinline) CTaskComplexWaitForBackup* Destructor() {
        this->CTaskComplexWaitForBackup::~CTaskComplexWaitForBackup();
        return this;
    }
    CTaskComplexWaitForBackup* ScalarDeletingDestructor(uint8 flags); // 0x697EF0 (lote 142A, banco Fn142A): destructor con borrado (ranura 0 de la vtable 0x870DA4), `ret 4`
};
