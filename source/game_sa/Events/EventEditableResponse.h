#pragma once

#include "Event.h"
#include <extensions/WEnum.hpp>

#include "TaskSimpleFacial.h"

class CPedGroup;

class NOTSA_EXPORT_VTABLE CEventEditableResponse : public CEvent {
public:
    CEventEditableResponse(eTaskType taskType = TASK_NONE/*notsa*/);
    ~CEventEditableResponse() override = default; // 0x4AC480

    CEvent* Clone() const noexcept override;
    bool HasEditableResponse() const override { return true; } // 0x420EF0;
    virtual CEventEditableResponse* CloneEditable() const noexcept = 0;

    bool WillRespond() const;
    void InformVehicleOccupants(CPed* ped);
    void InformRespectedFriends(CPed* ped);
    void InformGroup(CPed* ped);
    void TriggerLookAt(CPed* ped) const;
    void ComputeResponseTaskType(CPed* ped, bool bDecisionMakerTypeInGroup);
    void ComputeResponseTaskType(CPedGroup* pedGroup);
    bool ComputeResponseTaskOfType(CPed* ped, eTaskType taskId);

    auto GetFacialExpressionType() const { return (eFacialExpression)m_FacialExpressionType; }

private:
    friend void InjectHooksMain();
    friend class CEventHealthLow; // lote 197A (sin validar): su CloneEditable del EXE (0x5FF490) llama al cuerpo enganchado Constructor (0x4AC450)
    friend class CEventHealthReallyLow; // lote 197A (sin validar): constructor 0x5FF500 y CloneEditable 0x5FF550
    friend class CEventLowAngerAtPlayer; // lote 197A (sin validar): constructor 0x5FF5C0 y CloneEditable 0x5FF610
    friend class CEventHighAngerAtPlayer; // lote 197A (sin validar): constructor 0x5FF680 y CloneEditable 0x5FF6D0
    friend class CEventLeaderExitedCarAsDriver; // lote 156A (banco Fn156A): su CloneEditable del EXE (0x4B8340) llama al cuerpo enganchado Constructor (0x4AC450)
    static void InjectHooks();

    CEventEditableResponse* Constructor();

public:
    bool                               m_bAddToEventGroup;
    notsa::WEnumS16<eTaskType>         m_TaskId;
    notsa::WEnumS16<eFacialExpression> m_FacialExpressionType;
};
VALIDATE_SIZE(CEventEditableResponse, 0x14);
