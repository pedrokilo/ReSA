#pragma once

#include "eEventType.h"
#include <Vector.h>
#include "Base.h"

class CPed;
class CEntity;
class CPedGroup;
class CEventEditableResponse;

class NOTSA_EXPORT_VTABLE CEvent {
public:
    int32 m_nTimeActive;
    bool  m_bValid;

public:
    static void* operator new(unsigned size);
    static void operator delete(void* object);

    CEvent();

    virtual ~CEvent();
    virtual eEventType GetEventType() const = 0;
    virtual int32 GetEventPriority() const = 0;
    virtual int32 GetLifeTime() = 0;
    virtual CEvent* Clone() const noexcept = 0;
    virtual bool AffectsPed(CPed* ped);              // 0x4ABFE0 (lote 169A): `mov al,1; ret 4`; las vtables del EXE de todos los eventos que no la redefinen apuntan a esta direccion
    virtual bool AffectsPedGroup(CPedGroup* pedGroup); // 0x4ABFF0 (lote 169A): `mov al,1; ret 4`
    virtual bool IsCriminalEvent();                  // 0x420E90 (lote 137A): `xor al, al; ret`; las vtables del EXE de todos los eventos que no la redefinen apuntan a esta direccion
    virtual void ReportCriminalEvent(CPed* ped);     // 0x420EA0 (lote 137A): `ret 4` (vacia)
    virtual bool HasEditableResponse() const { return false; }
    virtual CEntity* GetSourceEntity() const { return nullptr; }
    virtual bool TakesPriorityOver(const CEvent& other) { return TakesPriorityOverBase(other); }
    bool TakesPriorityOverBase(const CEvent& other); // 0x4ABC30: cuerpo de la implementacion por defecto (las vtables del EXE apuntan a esta direccion)
    virtual float GetLocalSoundLevel();              // 0x420EB0 (lote 137A): `fld dword [0x858B50]` (0.0f); `ret`
    virtual bool DoInformVehicleOccupants(CPed* ped); // 0x420EC0 (lote 137A): `xor al, al; ret 4`
    virtual bool IsValid(CPed* ped);                 // 0x4AC020 (lote 169A): m_bValid o `m_nTimeActive <= GetLifeTime()` (llamada virtual de la ranura 3, con signo)
    virtual bool CanBeInterruptedBySameEvent();      // 0x4AC000 (lote 169A): `xor al,al; ret`

    float GetSoundLevel(const CEntity* entity, CVector& position);
    static float CalcSoundLevelIncrement(float level1, float level2);

    void UnTick() { m_nTimeActive--; }
    void Tick() { m_nTimeActive++; }

public: // Casting.hpp support //
    template<typename From, typename To>
    static constexpr bool classof(const From* f) {
        if constexpr (std::is_same_v<To, CEventEditableResponse>) {
            return f->HasEditableResponse();
        } else {
            return f->GetEventType() == To::Type;
        }
    }

private:
    friend void InjectHooksMain();
    friend class CEventEscalator; // lote 151A (banco Fn151A): su Clone del EXE (0x602B90) llama al cuerpo enganchado Constructor (0x4ABFC0)
    friend class CEventLeaderQuitEnteringCarAsDriver; // lote 156A (banco Fn156A): su Clone del EXE (0x63A150) llama al cuerpo enganchado Constructor (0x4ABFC0)
    static void InjectHooks();

    CEvent* Constructor();
};
VALIDATE_SIZE(CEvent, 0xC);
