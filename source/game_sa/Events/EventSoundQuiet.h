#pragma once

#include "EventEditableResponse.h"
#include "Vector.h"

class NOTSA_EXPORT_VTABLE CEventSoundQuiet final : public CEventEditableResponse {
public:
    CEntity* m_entity;
    float    m_fLocalSoundLevel;
    uint32   m_startTimeInMs;
    CVector  m_position;

public:
    CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position);
    ~CEventSoundQuiet() override;

    eEventType GetEventType() const override; // 0x5DEA60 (lote 165A, banco Fn165A)
    int32 GetEventPriority() const override; // 0x5DEAD0 (lote 165A)
    int32 GetLifeTime() override; // 0x5DEA70 (lote 165A)
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override; // 0x5DEA80 (lote 165A)
    float GetLocalSoundLevel() override; // 0x5DEA90 (lote 165A)
    bool CanBeInterruptedBySameEvent() override; // 0x5DEAA0 (lote 165A)
    CEventEditableResponse* CloneEditable() const noexcept override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventSoundQuiet* Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position);

};
