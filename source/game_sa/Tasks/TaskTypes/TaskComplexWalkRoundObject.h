#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundObject : public CTaskComplex {
public:
    int32        m_moveState;
    CVector      m_targetPoint;
    CEntity*     m_object;
    CPointRoute* m_pointRoute;
    int32        field_24;
    int32        field_28;
    int8         field_2C;
    int8         field_2D;
    // Como el original: el constructor (0x655020) no escribe estos campos; las uniones anónimas evitan el inicializador por defecto de CVector
    union { CVector m_objectStartPos; };     // posición inicial del objeto
    union { CVector m_objectStartForward; }; // frente inicial del objeto
    union { CVector m_objectStartRight; };   // derecha inicial del objeto

public:
    static constexpr auto Type = TASK_COMPLEX_WALK_ROUND_OBJECT;

    CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object);
    ~CTaskComplexWalkRoundObject() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexWalkRoundObject(m_moveState, m_targetPoint, m_object); }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateRouteTask(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexWalkRoundObject* Constructor(int32 moveState, const CVector& targetPoint, CEntity* object);
};

VALIDATE_SIZE(CTaskComplexWalkRoundObject, 0x54);
