#pragma once

#include "EventEditableResponse.h"

class C2dEffectPedAttractor;
class CEntity;

class NOTSA_EXPORT_VTABLE CEventAttractor : public CEventEditableResponse {
public:
    C2dEffectPedAttractor* m_2dEffect;
    CEntity*               m_entity;
    bool                   m_bAvoidLookingAtAttractor;

public:
    static void InjectHooks();

    CEventAttractor(C2dEffectPedAttractor* effect, CEntity* entity, bool bAvoidLookingAtAttractor, /* notsa => */eTaskType taskType = TASK_NONE);
    ~CEventAttractor() override;

    eEventType GetEventType() const override;     // 0x4AF3D0 (lote 187A, sin validar)
    int32 GetEventPriority() const override;      // 0x4AF3F0 (lote 187A, sin validar)
    int32 GetLifeTime() override;                 // 0x4AF3E0 (lote 187A, sin validar)
    bool AffectsPed(CPed* ped) override;
    CEventEditableResponse* CloneEditable() const noexcept override;

    static bool IsEffectActive(CEntity* entity, const C2dEffectPedAttractor* effect);
private:
    // 0x4AF350
    auto Constructor(C2dEffectPedAttractor* effect, CEntity* entity, bool bAvoidLookingAtAttractor) {
        return std::construct_at(this, effect, entity, bAvoidLookingAtAttractor);
    }
};
VALIDATE_SIZE(CEventAttractor, 0x20);
