#pragma once

#include "Event.h"

class CTask;

class NOTSA_EXPORT_VTABLE CEventScriptCommand : public CEvent {
public:
    int32  m_primaryTaskIndex;
    CTask* m_task;
    bool   m_affectsDeadPeds;

public:
    static constexpr auto Type = eEventType::EVENT_SCRIPT_COMMAND;

    CEventScriptCommand(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds = false);
    ~CEventScriptCommand() override;
    // 0x4B0A50: el EXE deja EAX = resultado del destructor de la tarea (o la cabeza de la cadena SEH), ECX = esa cabeza y EDX intacto.
    // Se declara __fastcall (ECX = this, EDX = el EDX de entrada, sin parametros en la pila: misma limpieza que el __thiscall sin
    // argumentos) para poder devolver EDX tal como entro.
    static uint32 __fastcall Destructor(CEventScriptCommand* self, uint32 entryEdx);

    eEventType GetEventType() const override { return Type; }
    int32 GetEventPriority() const override;
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() const noexcept override { auto* task = CloneScriptTask(); return new CEventScriptCommand(m_primaryTaskIndex, task, m_affectsDeadPeds); } // 0x4B6490 - Como el original: la tarea se clona antes de reservar el evento
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;
    bool IsValid(CPed* ped) override;
    virtual CTask* CloneScriptTask() const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventScriptCommand* Constructor(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds);

};

VALIDATE_SIZE(CEventScriptCommand, 0x18);
