#pragma once

#include "EventAttractor.h"

#include "EventEditableResponse.h"
#include "EventAttractor.h"
#include "Ragdoll/IKChainManager.h"


class NOTSA_EXPORT_VTABLE CEventScriptedAttractor : public CEventAttractor {
public:
    static void InjectHooks();

    // Como el original: el constructor de 0x5FEF40 solo recibe el efecto y la entidad (ret 8) y pasa false a CEventAttractor
    CEventScriptedAttractor(C2dEffectPedAttractor* the2dEffect, CEntity* entity);
    ~CEventScriptedAttractor() override = default;

    CEventScriptedAttractor* Constructor(C2dEffectPedAttractor* the2dEffect, CEntity* entity);
    eEventType GetEventType() const override { return EVENT_SCRIPTED_ATTRACTOR; }
    CEventScriptedAttractor* CloneEditable() const noexcept override { return new CEventScriptedAttractor(m_2dEffect, m_entity); }
};
VALIDATE_SIZE(CEventScriptedAttractor, 0x20);
