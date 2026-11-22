#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;
class CPatrolRoute;

// Reconstruccion parcial (lotes 195A y 210A, sin validar): campos tomados del constructor 0x674930, de la vtable 0x870114 y de las funciones 0x670A70, 0x670B00, 0x670D80 y 0x670E20. La clase sigue
// siendo abstracta (no declara Clone, MakeAbortable ni ControlSubTask): las funciones del EXE que ya estan en C++ se enganchan como S y sin declarar la virtual (la clase no tiene vtable exportada
// para RH_ScopedVMTInstall), con otro nombre cuando coincidiria con una virtual.
class NOTSA_EXPORT_VTABLE CTaskComplexFollowPatrolRoute : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_FOLLOW_PATROL_ROUTE;

    int16         m_nPatrolMode;      // +0x0C: 0 = una pasada, 1 = ida y vuelta una vez, 2 = ida y vuelta sin fin, 3 = circuito (ver GetNextSubTaskType)
    int16         m_nRoutePasses;     // +0x0E: pasadas completas de la ruta (contador que incrementa GetNextSubTaskType al llegar al final)
    int16         m_nMoveState;       // +0x10: estado de movimiento de las subtareas (eMoveState)
    int16         m_nCurrentNode;     // +0x12: indice del nodo actual de la ruta (con signo, sin comprobar el rango)
    float         m_fTargetRadius;    // +0x14: radio de llegada a cada punto
    float         m_fMoveStateRadius; // +0x18: radio del estado de movimiento del ultimo punto
    CPatrolRoute* m_pRoute;           // +0x1C: copia propia de la ruta (se libera con el destructor)
    uint32        m_nFlags;           // +0x20: bit 0 = primera vez (lo apaga CreateFirstSubTask), bit 1 = reanudar siguiendo nodos hasta el punto mas cercano
    uint8         m_padding[0xC];     // +0x24..+0x2F: sin uso conocido

    static void InjectHooks();


    eTaskType GetTaskType() const override { return Type; } // 0x670A60
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // CTask* ControlSubTask(CPed* ped) override;

    //! 0x670B00 (lote 210A, sin validar): crea la subtarea del tipo dado (nulo si el tipo no es de los soportados o no hay memoria)
    CTask* CreateSubTask(int32 taskType);
    //! 0x670A70 (lote 210A, sin validar): tipo de la siguiente subtarea segun el nodo actual y el modo (sin argumentos; puede invertir la ruta y reiniciar el nodo)
    eTaskType GetNextSubTaskType();
    //! 0x670D80 (lote 210A, sin validar): CreateNextSubTask del EXE (ranura 8 de la vtable 0x870114; la clase aun no declara sus virtuales, por eso el nombre con sufijo)
    CTask* CreateNextSubTaskBody(CPed* ped);
    //! 0x670E20 (lote 210A, sin validar): CreateFirstSubTask del EXE (ranura 9)
    CTask* CreateFirstSubTaskBody(CPed* ped);
};
VALIDATE_SIZE(CTaskComplexFollowPatrolRoute, 0x30);

//! 0x674930 (F4 de codigo vivo): constructor del EXE de la tarea (thiscall como fastcall; reserva en `self` memoria de CTask::operator new(0x30)); lo usa el comando TASK_FOLLOW_PATROL_ROUTE (2071).
CTaskComplexFollowPatrolRoute* __fastcall CTaskComplexFollowPatrolRouteConstructorExe(CTaskComplexFollowPatrolRoute* self, void* edx, int32 a, const CPatrolRoute* route, int32 b, uint32 c, uint32 d);
