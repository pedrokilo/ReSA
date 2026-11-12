#pragma once

#include "TaskComplexPartner.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPartnerChat : public CTaskComplexPartner {
public:
    int32 field_70;
    bool m_conversationEnabled;
    char field_75;
    bool m_pedConversationLoaded;

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER_CHAT;

    CTaskComplexPartnerChat(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, bool conversationEnabled, bool a8, CVector point);
    ~CTaskComplexPartnerChat() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override; // 0x684E30 (lote 185A, sin validar)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    virtual CTaskComplexSequence* GetPartnerSequence();

    static void InjectHooks();
    CTaskComplexPartnerChat* Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, bool conversationEnabled, bool a8, CVector point);
};
VALIDATE_SIZE(CTaskComplexPartnerChat, 0x78);
