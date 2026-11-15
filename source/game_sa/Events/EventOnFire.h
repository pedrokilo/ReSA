#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventOnFire : public CEvent {
public:
    CEventOnFire() = default;
    ~CEventOnFire() override = default;

    eEventType GetEventType() const override; // 0x5FF770 (lote 197A, sin validar)
    int32 GetEventPriority() const override; // 0x5FF890 (lote 197A, sin validar)
    int32 GetLifeTime() override; // 0x5FF780 (lote 197A, sin validar)
    CEventOnFire* Clone() const noexcept override; // 0x5FF790 (lote 197A, sin validar)
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override; // 0x5FF7E0 (lote 197A, sin validar)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventOnFire* Constructor(); // 0x5FF740 (lote 197A, sin validar)
};
VALIDATE_SIZE(CEventOnFire, 0xC);
