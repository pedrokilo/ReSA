#pragma once

#include "TaskComplex.h"
#include "Enums/eMoveState.h"

struct C2dEffectPedAttractor;
class CEntity;
class CPedAttractor;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffect : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT;

    static void InjectHooks();

    CTaskComplexUseEffect(C2dEffectPedAttractor* fx, CEntity* entity);
    CTaskComplexUseEffect(const CTaskComplexUseEffect&);
    ~CTaskComplexUseEffect() override;

    CTask*    Clone() const override { return new CTaskComplexUseEffect{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) final override;
    CTask*    CreateNextSubTask(CPed* ped) final override;
    CTask*    CreateFirstSubTask(CPed* ped) final override;
    CTask*    ControlSubTask(CPed* ped) final override;

public: // Constructor: lo llaman los constructores de las subclases reconstruidas del EXE (lote 115A)
    // 0x6321F0
    CTaskComplexUseEffect* Constructor(C2dEffectPedAttractor* fx, CEntity* entity) {
        this->CTaskComplexUseEffect::CTaskComplexUseEffect(fx, entity);
        return this;
    }

public: // Envoltura del cuerpo 0x632270 (con hook): la llaman los cuerpos reconstruidos de CTaskComplexUseEffectRunning (0x636850) y CTaskComplexUseEffectSprinting (0x636930; lote 141A, banco Fn141A), sin expansion en linea
    // 0x632270
    __declspec(noinline) CTaskComplexUseEffect* Destructor() {
        this->CTaskComplexUseEffect::~CTaskComplexUseEffect();
        return this;
    }

public:
    C2dEffectPedAttractor* m_2dFx{};
    CEntity*               m_Entity{};
    CPedAttractor*         m_PedAttractor{};
    CPed*                  m_Ped{};
    eMoveState             m_MoveState{ eMoveState::PEDMOVE_WALK };
    bool                   m_bAbort{};
};
