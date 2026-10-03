#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"
#include <Audio/Enums/PedSpeechContexts.h>

class CEvent;
class CTaskSimpleSay;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleSay : public CTaskSimple {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_SAY;

    CTaskSimpleSay(eGlobalSpeechContext gctx, int32 sayDuration = -1);
    CTaskSimpleSay(const CTaskSimpleSay& o) : CTaskSimpleSay{o.m_GlobalSpeechContext, o.m_Duration} {
        static_cast<int32&>(m_GlobalSpeechContext) = o.m_GlobalSpeechContext.get_underlying(); // Como el original: Clone copia el dword completo
    }
    ~CTaskSimpleSay() = default;

    CTask* Clone() const override { return new CTaskSimpleSay(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private:
    eGlobalSpeechContextS32 m_GlobalSpeechContext{};
    int32                   m_Duration{};
    CTaskTimer              m_Timer; // Como el original: sin inicializacion por valor (el relleno 0x1A-0x1B queda sin escribir)

private: // Wrappers for hooks
    // 0x48E360
    CTaskSimpleSay* Constructor(int32 sayId, int32 sayDuration) {
        this->CTaskSimpleSay::CTaskSimpleSay((eGlobalSpeechContext)sayId, sayDuration);
        static_cast<int32&>(m_GlobalSpeechContext) = sayId; // Como el original: el contexto se copia como dword completo
        return this;
    }
    // 0x48E4E0
    CTaskSimpleSay* Destructor() {
        this->CTaskSimpleSay::~CTaskSimpleSay();
        return this;
    }
};
