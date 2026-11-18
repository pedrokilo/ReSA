#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CEntity;
class CObject;

class NOTSA_EXPORT_VTABLE CTaskComplexGotoDoorAndOpen : public CTaskComplex {
public:
    CObject* m_Object;
    CVector  m_Start;
    CVector  m_End;
    uint32   m_nStartTime;
    uint32   m_nOffsetTime;
    uint8    byte30;
    bool     m_bRefreshTime;
    uint8    _align[2];
    uint8    m_nFlags;

public:
    static constexpr auto Type = TASK_COMPLEX_GOTO_DOOR_AND_OPEN;

    explicit CTaskComplexGotoDoorAndOpen(CObject* door);
    CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end);
    ~CTaskComplexGotoDoorAndOpen() override;

    eTaskType GetTaskType() const override; // 0x66BB90 (lote 191A, sin validar)
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    /*!
     * @addr 0x66BB20
     */
    CTaskComplexGotoDoorAndOpen* Constructor(CObject* door) { this->CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(door); return this; }
    /*!
     * @addr 0x66BBA0
     */
    CTaskComplexGotoDoorAndOpen* Constructor(CVector const& start, CVector const& end) { this->CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(start, end); return this; }
public:
    /*!
     * @addr 0x66BC00
     */
    __declspec(noinline) CTaskComplexGotoDoorAndOpen* Destructor() { this->CTaskComplexGotoDoorAndOpen::~CTaskComplexGotoDoorAndOpen(); return this; }
};
VALIDATE_SIZE(CTaskComplexGotoDoorAndOpen, 0x38);
