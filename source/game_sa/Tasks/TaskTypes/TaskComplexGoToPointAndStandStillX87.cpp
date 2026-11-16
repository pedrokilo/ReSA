// Archivo propio de CTaskComplexGoToPointAndStandStill::SelectMoveState (0x668250) y CreateSubTask (0x6682D0): aritmetica x87 del EXE (24 bits, intermedios en registros con exponente ampliado, un SSE con DAZ/FTZ aplana
// los denormales que la x87 conserva); se compila con /O2 /Ob0 /Oi /arch:IA32 /fp:fast (ver source/CMakeLists.txt) para que el resto de la clase conserve la aritmetica normal. Sus hooks siguen en TaskComplexGoToPointAndStandStill.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskComplexGoToPointAndStandStill.h"

#include "TaskSimpleGoToPoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"

#pragma float_control(precise, on, push)
// 0x668250 (lote 198A, sin validar; thiscall con la tarea como `this` sin usar, cuatro argumentos de pila, `ret 10h`, 116 bytes): distancia 2D al cuadrado (ped - destino) = dx*dx + dy*dy (x87, 24 bits: cada producto y la suma se redondean a float)
// y dos comparaciones ordenadas (`fcompp; test ah,5; jp`: solo se toma la rama de "menor" si la comparacion es ordenada y menor; con NaN gana la rama contraria): dist < radio^2 -> PEDMOVE_WALK; si no, dist < radioCorrer^2 -> PEDMOVE_RUN; si no,
// PEDMOVE_SPRINT (con NaN en la distancia: SPRINT). Escribe solo m_moveState (+8) de la subtarea. Los cuadrados se calculan como float*float.
__declspec(noinline) void CTaskComplexGoToPointAndStandStill::SelectMoveState(CTaskSimpleGoToPoint* gotoPointTask, CPed* ped, float fMoveStateRadius, float fRunOrSprintRadius) {
    const CVector& pedPos = ped->GetPosition();
    // Los intermedios son `double`: la x87 del juego trabaja con la precision de 24 bits (cada operacion se redondea a 24 bits) y con exponente ampliado; un `double` conserva ese valor sin volver a
    // redondearlo al rango de un float (un `float` lo desbordaria a infinito donde el EXE sigue con un valor finito) y el EXE no guarda ninguno de ellos en memoria.
    const double dx = static_cast<double>(pedPos.x) - static_cast<double>(gotoPointTask->m_vecTargetPoint.x);
    const double dy = static_cast<double>(pedPos.y) - static_cast<double>(gotoPointTask->m_vecTargetPoint.y);
    const double dist2DSq = dx * dx + dy * dy;
    const double radius1 = fMoveStateRadius;
    const double radius2 = fRunOrSprintRadius;
    if (dist2DSq < radius1 * radius1) {
        gotoPointTask->m_moveState = PEDMOVE_WALK;
    } else if (dist2DSq < radius2 * radius2) {
        gotoPointTask->m_moveState = PEDMOVE_RUN;
    } else {
        gotoPointTask->m_moveState = PEDMOVE_SPRINT;
    }
}

// 0x6682D0 (lote 198A, sin validar; thiscall, dos argumentos de pila, `ret 8`, con SEH que solo libera la memoria si el constructor lanzara): tipos 0x516 (TASK_FINISHED) y cualquier otro no listado -> nulo (el EXE no tiene rama de error).
// TASK_SIMPLE_STAND_STILL con bStopExactly y subtarea GO_TO_POINT: velocidad de mezcla segun la velocidad local Y de la animacion; las comparaciones del EXE son `fcomp; test ah,5/41h; jp/jne`: shiftY < 0.01 (ordenada) -> 8.0f; si no, dot <= 0.01 o NaN
// -> 16.0f; si no, 50 / ((dot / (shiftY * 0.5) - 1) * paso), y solo si es MAYOR que 16.0 (ordenada) se recorta a 16.0f.
__declspec(noinline) CTask* CTaskComplexGoToPointAndStandStill::CreateSubTask(eTaskType taskType, CPed* ped) {
    // Como el original: cada rama reserva con CTask::operator new (0x61A5A0); sin memoria devuelve nulo sin construir; si no, devuelve el EAX del constructor enganchado de la subtarea
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT: {
        auto* const memory = static_cast<CTaskSimpleGoToPoint*>(CTask::operator new(sizeof(CTaskSimpleGoToPoint)));
        auto* const tGoToPoint = memory ? memory->Constructor(m_moveState, m_vecTargetPoint, m_fRadius, false, m_bMustOverShootTarget) : nullptr; // 0x667CD0 (con hook)

        if (m_bTrackingEntity)
            tGoToPoint->gotoPointFlags.m_b05 = true;

        if (m_moveState == PEDMOVE_RUN)
            SelectMoveState(tGoToPoint, ped, m_fMoveStateRadius, 100'000'000.0f);

        else if (m_moveState == PEDMOVE_SPRINT)
            SelectMoveState(tGoToPoint, ped, m_fMoveStateRadius, 10.0f);

        return tGoToPoint;
    }
    case TASK_COMPLEX_LEAVE_CAR: {
        auto* const memory = static_cast<CTaskComplexLeaveCar*>(CTask::operator new(sizeof(CTaskComplexLeaveCar)));
        return memory ? memory->Constructor(ped->m_pVehicle, 0, 0, true, false) : nullptr; // 0x63B8C0 (con hook)
    }
    case TASK_SIMPLE_PAUSE: {
        auto* const memory = static_cast<CTaskSimplePause*>(CTask::operator new(sizeof(CTaskSimplePause)));
        return memory ? memory->Constructor(1) : nullptr; // 0x48E750 (con hook)
    }
    case TASK_SIMPLE_STAND_STILL: {
        if (m_bStopExactly && m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            CVector vecDistance; // temporal local sin inicializar: la escribe CVector::VectorSubtract (0x40FE60, con hook) con (salida, destino, posicion del peaton) como el EXE
            VectorSubtract(&vecDistance, &m_vecTargetPoint, &ped->GetPosition());
            float   fDotProduct = DotProduct(&vecDistance, &ped->GetForward());
            float   fBlendDelta = 8.0f;
            // Como el original (x87 de 24 bits, intermedios en registros con exponente ampliado: `double`): el valor sin redondear a float es el que se compara con 16.0; el que se guarda es el redondeado a float (fst)
            const double shiftY = ped->m_vecAnimMovingShiftLocal.y;
            const double dot    = fDotProduct;
            if (!(shiftY < 0.01f)) { // con NaN entra aqui (el EXE solo salta con "menor" ordenado)
                if (!(dot > 0.01f)) {
                    fBlendDelta = 16.0f;
                } else {
                    const double blend = 50.0 / ((dot / (shiftY * 0.5) - 1.0) * static_cast<double>(CTimer::GetTimeStep()));
                    fBlendDelta        = static_cast<float>(blend);
                    if (blend > 16.0)
                        fBlendDelta = 16.0f;
                }
            }
            auto* const memory = static_cast<CTaskSimpleStandStill*>(CTask::operator new(sizeof(CTaskSimpleStandStill)));
            return memory ? memory->Constructor(2000, false, true, fBlendDelta) : nullptr; // 0x62F310 (con hook)
        }
        auto* const memory = static_cast<CTaskSimpleStandStill*>(CTask::operator new(sizeof(CTaskSimpleStandStill)));
        return memory ? memory->Constructor(1, false, false, 8.0f) : nullptr;
    }
    default:
        return nullptr; // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nulo
    }
}

#pragma float_control(pop)
