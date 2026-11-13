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
    eEventType GetEventType() const override; // 0x5FEF80 (lote 188A, sin validar)
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x5FEF90 (lote 188A, sin validar)
};
VALIDATE_SIZE(CEventScriptedAttractor, 0x20);
